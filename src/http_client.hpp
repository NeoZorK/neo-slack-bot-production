#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <string>
#include <map>
#include <memory>
#include "interfaces/i_http_client.hpp"

// Forward declaration to avoid including large header in .hpp
namespace httplib {
    class Client;
}

/**
 * @brief HTTP client for API requests
 * 
 * Thread Safety: NOT thread-safe. Each instance should be used from a single thread.
 * External synchronization required if accessed from multiple threads.
 */
class HttpClient : public IHttpClient {
public:
    /**
     * @brief Construct HTTP client
     */
    HttpClient();
    
    /**
     * @brief Destructor
     */
    ~HttpClient();
    
    /**
     * @brief Perform HTTP GET request
     * @param url URL to request
     * @param response Output parameter for response body
     * @return true if request successful, false otherwise
     */
    bool get(const std::string& url, std::string& response);
    
    /**
     * @brief Perform HTTP POST request
     * @param url URL to request
     * @param data Request body data
     * @param response Output parameter for response body
     * @return true if request successful, false otherwise
     */
    bool post(const std::string& url, const std::string& data, std::string& response);
    
    /**
     * @brief Set HTTP header
     * @param key Header name
     * @param value Header value
     */
    void set_header(const std::string& key, const std::string& value);
    
    /**
     * @brief Set request timeout
     * @param seconds Timeout in seconds
     */
    void set_timeout(int seconds);

private:
    std::map<std::string, std::string> headers_;
    int timeout_seconds_;
    std::unique_ptr<httplib::Client> client_;
    
    void apply_headers();
    std::pair<std::string, std::string> parse_url(const std::string& url);
};

#endif // HTTP_CLIENT_HPP
