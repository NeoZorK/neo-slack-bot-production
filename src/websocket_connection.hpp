#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include <string>

/**
 * @brief WebSocket connection management utilities
 * 
 * Provides low-level functions for establishing TCP/TLS connections
 * and performing WebSocket handshake according to RFC 6455.
 */
namespace websocket_connection {
    /**
     * @brief Connection parameters for WebSocket handshake
     */
    struct ConnectionParams {
        const char* host;      ///< Hostname or IP address
        const char* path;      ///< WebSocket path
        int port;              ///< Port number (typically 443 for WSS)
        std::string token;     ///< Authentication token
    };
    
    /**
     * @brief Establish TCP connection to host
     * @param socket_fd Output parameter for socket file descriptor
     * @param host Hostname or IP address
     * @param port Port number
     * @return true if connection successful, false otherwise
     */
    bool establish_tcp_connection(int& socket_fd, const char* host, int port);
    
    /**
     * @brief Establish TLS/SSL connection over TCP socket
     * @param ssl_ctx Output parameter for SSL context
     * @param ssl Output parameter for SSL connection
     * @param socket_fd TCP socket file descriptor
     * @return true if TLS connection successful, false otherwise
     */
    bool establish_tls_connection(void*& ssl_ctx, void*& ssl, int socket_fd);
    
    /**
     * @brief Generate WebSocket key for handshake
     * @return Base64-encoded random key
     */
    std::string generate_websocket_key();
    
    /**
     * @brief Perform WebSocket handshake
     * @param ssl SSL connection handle
     * @param params Connection parameters
     * @return true if handshake successful, false otherwise
     */
    bool perform_websocket_handshake(void* ssl, const ConnectionParams& params);
    
    /**
     * @brief Cleanup connection resources
     * @param ssl_ctx SSL context (set to nullptr after cleanup)
     * @param ssl SSL connection (set to nullptr after cleanup)
     * @param socket_fd Socket file descriptor (set to -1 after cleanup)
     */
    void cleanup_connection(void*& ssl_ctx, void*& ssl, int& socket_fd);
}

#endif // WEBSOCKET_CONNECTION_HPP
