#ifndef CRPC_HTTP_HEADER_H
#define CRPC_HTTP_HEADER_H

#include <string>
#include <unordered_map>

namespace crpc
{

#define HTTP_METHOD         "HTTP_METHOD"
#define HTTP_URL_PATH       "HTTP_URL_PATH"
#define HTTP_VERSION        "HTTP_VERSION"
#define CONTENT_LENGTH      "Content-Length"
#define CONNECTION          "Connection"

class HttpHeader
{
public:

    HttpHeader()
    {}

    void reset()
    {
        _http_header_map = std::unordered_map<std::string, std::string>();
    }

    void append_header(const std::string& key, const std::string& value)
    {
        _http_header_map[key] = value;
    }

    const std::string get_header(const std::string& key)
    {
        return _http_header_map[key];
    }

    int64_t get_content_length()
    {
        return get_header_int64(CONTENT_LENGTH);
    }

    //like content-length
    int64_t get_header_int64(const std::string& key)
    {
        if (_http_header_map.find(key) == _http_header_map.end())
            return 0;

        return strtol(_http_header_map[key].c_str(), NULL, 0);
    }

    
    void dump_http_header()
    {
        for (auto itr = _http_header_map.begin(); itr != _http_header_map.end(); ++itr)
        {
            crpc_log("key: %s value : %s", itr->first.c_str(), itr->second.c_str());
        }
    }
private:
    std::unordered_map<std::string, std::string> _http_header_map;
};

};

#endif
