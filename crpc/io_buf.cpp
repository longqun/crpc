#include <sys/uio.h>
#include "io_buf.h"
#include "crpc_log.h"
namespace crpc
{

#define TLS_MAX_BLOCK (20)

//回收的时候需要保障block 被reset了。
class TlsBlockManager
{
public:
    TlsBlockManager():_header(NULL), _block_size(0)
    {}

    ~TlsBlockManager()
    {
        block_back_to_system(0);
    }

    Block* alloc_block()
    {
        if (!_header)
            return new Block();

        _block_size--;
        Block* ret = _header;
        _header = _header->next;
        ret->reset();
        return ret;
    }

    //回收block
    void retrieve_block(Block* block)
    {
        Block* cur = block;
        while (cur)
        {
            Block* next = cur->next;
            cur->next = _header;
            _header = cur;
            _header->reset();
            ++_block_size;
            cur = next;
        }

        block_back_to_system();
    }
private:

    void block_back_to_system(int max_block = TLS_MAX_BLOCK)
    {
        while ((int)_block_size > max_block && _block_size)
        {
            --_block_size;
            Block* del_ptr = _header;
            _header = _header->next;
            delete del_ptr;
        }
    }

    Block* _header;
    size_t _block_size;
};

static __thread TlsBlockManager* s_tls_block_manager;

static TlsBlockManager* get_tls_block_manager()
{
    if (!s_tls_block_manager)
        s_tls_block_manager = new TlsBlockManager;
    return s_tls_block_manager;
}

IoBuf::IoBuf():_buf_size(0), _header(NULL), _tail(NULL)
{
    _alloc_default_block();
}

IoBuf::~IoBuf()
{
    get_tls_block_manager()->retrieve_block(_header);
}

Block* IoBuf::pop_header()
{
    if (!_header)
        return NULL;

    Block* ret = _header;
    _header = _header->next;
    if (_header == NULL)
    {
        _header = _tail = NULL;
    }

    ret->next = NULL;
    return ret;
}

void IoBuf::push_tail()
{
    if (!_header)
    {
        _header = _tail = get_tls_block_manager()->alloc_block();
    }
    else
    {
        _tail->next = get_tls_block_manager()->alloc_block();
        _tail = _tail->next;
    }
}

void IoBuf::_alloc_default_block()
{
    if (_header == NULL)
    {
        _header = _tail = get_tls_block_manager()->alloc_block();
    }
}

std::string IoBuf::to_string(int n)
{
    if (n == 0)
        return std::string();

    size_t len = std::min(n, (int)_buf_size);
    std::string ret(len, '\0');
    cutn(&ret[0], n);
    return ret;
}

//解决查找字符串跨块
struct IoBufChar
{
    Block* cur;
    //总偏移
    int offset;
    //当前块偏移
    int block_index;

    IoBufChar(Block* block):cur(block),offset(0),block_index(cur ? cur->read_index : 0)
    {}

    bool has_next()
    {
        if (!cur)
            return false;

        while (cur)
        {
            if (block_index < cur->write_index)
                return true;

            cur = cur->next;
            block_index = cur ? cur->read_index : 0;
        }
        return false;
    }

    void next()
    {
        assert(cur);
        if(!has_next())
        {
            crpc_log("parse buf failed, no next!");
            return;
        }

        offset++;
        while (cur)
        {
            if (block_index < cur->write_index)
            {
                ++block_index;
                return;
            }
            cur = cur->next;
            block_index = cur ? cur->read_index : 0;
        }
    }

    unsigned char ch()
    {
        return cur->buff[block_index];
    }
};

#define CR '\r'
#define LF '\n'
bool IoBuf::cut_crlf(std::string& str)
{
    bool found = false;

    IoBufChar io_ch(_header);
    while (io_ch.has_next())
    {
        if (io_ch.ch() == CR && io_ch.has_next())
        {
            io_ch.next();
            if (io_ch.ch() == LF)
            {
                found = true;
                break;
            }
        }

        if (io_ch.has_next())
            io_ch.next();
    }

    if (!found)
        return found;

    str = std::string();

    //裁剪/r/n之前的字符串,最后/n未计算进去
    int cut_size = io_ch.offset - 1;

    assert(cut_size >= 0);
    if (cut_size)
        str = to_string(cut_size);

    //移除/r/n
    char buf2[2];
    cutn(buf2, sizeof(buf2));

    return found;
}

void IoBuf::pop_n(int n)
{

}

int IoBuf::cutn(void * out, int n)
{
    char* out_ptr = (char *)out;
    int ret_len, cut_size;

    ret_len = cut_size = std::min(n, (int)_buf_size);

    while (cut_size)
    {
        assert(_header && _tail);
        Block* cur = _header;
        cut_size -= cur->cutn(out_ptr, cut_size);

        if (cur->read_size() == 0)
        {
            get_tls_block_manager()->retrieve_block(pop_header());
        }
    }

    _buf_size -= ret_len;
    return ret_len;
}

int IoBuf::copyn(void * out, int n)
{
    assert(n >= 0);
    int ret_n = std::min(n, (int)_buf_size);
    char* out_ptr = (char*)out;
    while (n)
    {
        Block* cur = _header;

        size_t cur_len = cur->copyn(out_ptr, n);
        out_ptr += cur_len;
        n -= cur_len;
    }
    return ret_n;
}

void IoBuf::append(IoBuf* io_buf)
{
    assert(io_buf);
    if (!io_buf->size())
        return;

    //皆大欢喜
    if (_header == NULL)
    {
        _header = io_buf->_header;
        _tail = io_buf->_tail;
        _buf_size = io_buf->_buf_size;

        io_buf->reset();
        return;
    }

    _tail->next = io_buf->_header;
    _tail = io_buf->_tail;
    _buf_size += io_buf->_buf_size;

    io_buf->reset();
}

void IoBuf::append(const void* data, int count)
{
    _alloc_default_block();

    while (count > 0)
    {
        assert(_tail);
        //cp data to last
        Block* end = _tail;
        size_t size = end->append(data, count);
        _buf_size += size;
        count -= size;

        //still has data ? alloc block
        if (!end->avalible_size())
            push_tail();
    }
}

const int MAX_APPEND_IOVEC = 10;
const int MAX_ALLOC_BLOCK = 10;
const int MAX_IOVEC_READ_SIZE = MAX_APPEND_IOVEC *  BLOCK_SIZE;

//TODO use readv 优化
int IoBuf::read_fd(int fd, int size)
{
    assert(size > 0);
    int vec_len = 1;
    iovec vec[MAX_APPEND_IOVEC];
    size = std::min(size, MAX_IOVEC_READ_SIZE);

    //get last block size  
    _alloc_default_block();

    vec[0].iov_base = &_tail->buff[_tail->read_index];
    vec[0].iov_len = std::min((int)_tail->avalible_size(), size);

    size -= _tail->avalible_size();
    Block* alloc_header,* cur;
    alloc_header = cur = NULL;
    for (; size > 0;)
    {
        Block* block = get_tls_block_manager()->alloc_block();
        if (!alloc_header)
        {
            alloc_header = block;
        }
        else if (cur)
        {
            cur->next = block;
        }

        cur = block;
        vec[vec_len].iov_base = &cur->buff[cur->write_index];
        vec[vec_len].iov_len = std::min((int)cur->avalible_size(), size);
        size -= cur->avalible_size();
    }

    int nr = readv(fd, vec, vec_len);
    if (nr > 0)
    {
        //handle vec[0]
        int total_len = nr;
        int block_read_len = std::min((int)_tail->avalible_size(), total_len);

        //has copy to back here
        _tail->write_index += block_read_len;
        total_len -= block_read_len;

        while (total_len > 0 )
        {
            block_read_len = std::min(total_len, (int)alloc_header->avalible_size());
            alloc_header->write_index += block_read_len;

            //append block to tail
            _tail->next = alloc_header;
            alloc_header = alloc_header->next;

            _tail = _tail->next;
            _tail->next = NULL;

            total_len -= block_read_len;
        }

        //assert check
        assert(total_len == 0);

        _buf_size += nr;
    }

    get_tls_block_manager()->retrieve_block(alloc_header);
    return nr;
}

int IoBuf::write_fd(int fd, int size)
{
    assert(size >= 0);

    if (!_buf_size)
        return 0;

    int vec_len = 0;
    iovec vec[MAX_APPEND_IOVEC];

    Block* cur = _header;
    for (;cur != NULL;)
    {
        int block_write_size = std::min((int)cur->read_size(), size);
        vec[vec_len].iov_base = &cur->buff[cur->read_index];
        vec[vec_len].iov_len = block_write_size;
        size -= block_write_size;
        vec_len++;
        if (size < 0)
        {
            break;
        }
        cur = cur->next;
    }

    int nr = writev(fd, vec, vec_len);
    if (nr <= 0)
        return nr;

    int total_len = nr;
    while (total_len > 0 && _header)
    {
        int block_write_size = std::min(total_len, (int)_header->read_size());
        total_len -= block_write_size;

        _header->read_index += block_write_size;
        if (_header->read_size() == 0)
        {
            get_tls_block_manager()->retrieve_block(pop_header());
        }
    }
    assert(total_len == 0);
    _buf_size -= nr;
    return nr;
}

}
