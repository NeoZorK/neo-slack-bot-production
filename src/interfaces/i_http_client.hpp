#ifndef I_HTTP_CLIENT_HPP
#define I_HTTP_CLIENT_HPP

#include <string>

/**
 * @brief HTTP client interface
 * 
 * Provides abstraction for HTTP client implementations.
 * Used for dependency inversion and testability.
 * 
 * Thread Safety: Implementation-dependent. Each implementation
 * should document its thread safety guarantees.
 */
class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    
    /**
     * @brief Perform HTTP GET request
     * @param url URL to request
     * @param response Output parameter for response body
     * @return true if request successful, false otherwise
     */
    virtual bool get(const std::string& url, std::string& response) = 0;
    
    /**
     * @brief Perform HTTP POST request
     * @param url URL to request
     * @param data Request body data
     * @param response Output parameter for response body
     * @return true if request successful, false otherwise
     */
    virtual bool post(const std::string& url, const std::string& data, 
                     std::string& response) = 0;
    
    /**
     * @brief Set HTTP header
     * @param key Header name
     * @param value Header value
     */
    virtual void set_header(const std::string& key, const std::string& value) = 0;
    
    /**
     * @brief Set request timeout
     * @param seconds Timeout in seconds
     */
    virtual void set_timeout(int seconds) = 0;
};

#endif // I_HTTP_CLIENT_HPP
