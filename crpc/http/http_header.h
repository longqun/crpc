#ifndef CRPC_HTTP_HEADER_H
#define CRPC_HTTP_HEADER_H

#include <string>
#include <unordered_map>
#include "common_header.h"
#include "crpc_log.h"

namespace crpc
{

#define HTTP_URL_PATH       "HTTP_URL_PATH"
#define HTTP_VERSION        "HTTP_VERSION"
#define CONTENT_LENGTH      "Content-Length"
#define CONNECTION          "Connection"


#define HTTP_RESPONSE_HTTP_TYPE         "HTTP/1.1 200 OK\r\n"
#define HTTP_RESPONSE_SERVER            "Server: crpc\r\n"
#define HTTP_RESPONSE_CONTENT_TYPE      "Content-Type: text/html; charset=utf-8\r\n"
#define HTTP_RESPONSE_CONNETION         "Connection: close\r\n"
#define HTTP_RESPONSE_CONNTENT_LEN      "Content-Length: %d\r\n"


#define MAX_HTTP_HEADER_SIZE (8 * 1024 * 1024)

class HttpHeader
{
public:

    HttpHeader():_max_header_size(MAX_HTTP_HEADER_SIZE),_header_size(0)
    {}

    void reset()
    {
        _http_header_map = std::unordered_map<std::string, std::string>();
    }

    void append_header(const std::string& key, const std::string& value)
    {
        _http_header_map[key] = value;
    }

    bool has_header(const std::string& key) const
    {
        return _http_header_map.find(key) != _http_header_map.end();
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

    #define CRLF "\r\n"
    std::string to_string()
    {
        std::string str;
        //header
        for (auto &kv : _http_header_map)
        {
            str += kv.first;
            str += ": ";
            str += kv.second;
            str += CRLF;
        }
        str += CRLF;
        return str;
    }

        //支持解析的最大http头
    size_t _max_header_size;
    size_t _header_size;

private:

    std::unordered_map<std::string, std::string> _http_header_map;
};

};

#endif
