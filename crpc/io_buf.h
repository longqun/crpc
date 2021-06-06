#ifndef IO_BUF_H
#define IO_BUF_H

#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <list>
#include <assert.h>
#include <atomic>
#include "crpc_log.h"

namespace crpc
{
struct Block
{
#define BLOCK_SIZE (4096)
    Block():read_index(0), write_index(0), buff(BLOCK_SIZE),next(NULL)
    {
        //static std::atomic<int> alloc_count(0);
        //crpc_log("alloc block %d", alloc_count++);
    }

    ~Block()
    {
        //static std::atomic<int> free_count(0);
        //crpc_log("free block %d", free_count++);
    }

    //can write size
    size_t avalible_size() const
    {
        return buff.size() - write_index;
    }

    int read_fd(int fd, int   size)
    {
        int ret = read(fd, &buff[write_index], std::min(size, (int)avalible_size()));
        if (ret > 0)
        {
            write_index += ret;
        }
        return ret;
    }

    //can read size
    size_t read_size() const
    {
        return write_index - read_index;
    }

    //fill out
    size_t cutn(void* out, size_t len)
    {
        size_t cut_size = std::min(len, read_size());
        memcpy(out, &buff[read_index], cut_size);
        read_index += cut_size;
        return cut_size;
    }

    size_t copyn(void* out, size_t len)
    {
        size_t copy_size = std::min(len, read_size());
        memcpy(out, &buff[read_index], copy_size);
        return copy_size;
    }

    void reset()
    {
        read_index = write_index = 0;
        next = NULL;
    }

    //copy to iobuf
    size_t append(const void* data, size_t len)
    {
        size_t copy_size = std::min(len, avalible_size());

        memcpy(&buff[write_index], data, copy_size);
        write_index += copy_size;
        return copy_size;
    }

    size_t read_index;
    size_t write_index;
    std::vector<char> buff;
    Block* next;
};

class IoBuf
{
public:
    IoBuf();

    ~IoBuf();

    int cutn(void *out, int n);

    int copyn(void* out, int n);

    void pop_n(int n);

    bool cut_crlf(std::string& str);

    void append(IoBuf* io_buf);

    void append(const void* data, int count);

    //read from fd
    int read_fd(int fd, int size = 4096);

    //write data to fd
    int write_fd(int fd, int size = 4096);

    size_t size() const
    {
        return _buf_size;
    }

    std::string to_string(int n);

private:

    void reset()
    {
        _header = _tail = NULL;
        _buf_size = 0;
    }

    Block* pop_header();

    void push_tail();

    void _alloc_default_block();

    size_t _buf_size;
    Block* _header;
    Block* _tail;
};

}

#endif
