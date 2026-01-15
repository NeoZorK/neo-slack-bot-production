#include "websocket_connection.hpp"
#include "logger.hpp"
#include <cstring>
#include <random>
#include <sstream>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <io.h>
    #define close closesocket
    #define SHUT_RDWR SD_BOTH
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/select.h>
#endif

#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <thread>
#include <chrono>

namespace websocket_connection {
    bool establish_tcp_connection(int& socket_fd, const char* host, int port) {
        struct addrinfo hints, *result, *rp;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        
        std::string port_str = std::to_string(port);
        int ret = getaddrinfo(host, port_str.c_str(), &hints, &result);
        if (ret != 0) {
            Logger::get_instance().error("getaddrinfo failed for " + std::string(host) + 
                                       ":" + port_str + ": " + gai_strerror(ret));
            return false;
        }

        for (rp = result; rp != nullptr; rp = rp->ai_next) {
            socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (socket_fd >= 0) {
#ifndef _WIN32
                int flags = fcntl(socket_fd, F_GETFL, 0);
                if (flags >= 0) {
                    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
                }
#endif
                
                int connect_result = ::connect(socket_fd, rp->ai_addr, rp->ai_addrlen);
                if (connect_result == 0) {
                    Logger::get_instance().debug("TCP connection established to " + 
                                                std::string(host) + ":" + port_str);
                    break;
                }
#ifndef _WIN32
                else if (errno == EINPROGRESS) {
                    fd_set write_fds;
                    FD_ZERO(&write_fds);
                    FD_SET(socket_fd, &write_fds);
                    struct timeval timeout;
                    timeout.tv_sec = 5;
                    timeout.tv_usec = 0;
                    
                    int select_result = select(socket_fd + 1, nullptr, &write_fds, nullptr, &timeout);
                    if (select_result > 0 && FD_ISSET(socket_fd, &write_fds)) {
                        int so_error = 0;
                        socklen_t len = sizeof(so_error);
                        if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &so_error, &len) == 0 && so_error == 0) {
                            Logger::get_instance().debug("TCP connection established to " + 
                                                        std::string(host) + ":" + port_str);
                            break;
                        }
                    }
                }
#endif
                close(socket_fd);
                socket_fd = -1;
            }
        }
        freeaddrinfo(result);
        
        if (socket_fd < 0) {
            Logger::get_instance().error("Failed to establish TCP connection to " + 
                                       std::string(host) + ":" + port_str);
        }
        
        return socket_fd >= 0;
    }
    
    bool establish_tls_connection(void*& ssl_ctx, void*& ssl, int socket_fd) {
        if (ssl_ctx) SSL_CTX_free(static_cast<SSL_CTX*>(ssl_ctx));
        ssl_ctx = SSL_CTX_new(TLS_client_method());
        if (!ssl_ctx) {
            Logger::get_instance().error("Failed to create SSL context");
            return false;
        }
        
        // Set default verification paths
        SSL_CTX_set_default_verify_paths(static_cast<SSL_CTX*>(ssl_ctx));
        
        ssl = SSL_new(static_cast<SSL_CTX*>(ssl_ctx));
        if (!ssl) {
            Logger::get_instance().error("Failed to create SSL object");
            return false;
        }
        
        SSL_set_fd(static_cast<SSL*>(ssl), socket_fd);
        
        // SSL_connect() in non-blocking mode may return SSL_ERROR_WANT_READ/WANT_WRITE
        // Need to retry until connection is established
        int ret = SSL_connect(static_cast<SSL*>(ssl));
        while (ret <= 0) {
            int ssl_error = SSL_get_error(static_cast<SSL*>(ssl), ret);
            
            if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
                // Wait for socket to be ready
                fd_set read_fds, write_fds;
                FD_ZERO(&read_fds);
                FD_ZERO(&write_fds);
                FD_SET(socket_fd, &read_fds);
                FD_SET(socket_fd, &write_fds);
                
                struct timeval timeout;
                timeout.tv_sec = 5;
                timeout.tv_usec = 0;
                
                int select_result = select(socket_fd + 1, &read_fds, &write_fds, nullptr, &timeout);
                if (select_result > 0) {
                    // Retry SSL_connect()
                    ret = SSL_connect(static_cast<SSL*>(ssl));
                    continue;
                } else {
                    // Timeout
                    Logger::get_instance().error("SSL_connect timeout");
                    return false;
                }
            } else {
                // Real error
                unsigned long err = ERR_get_error();
                char error_buf[256];
                ERR_error_string_n(err, error_buf, sizeof(error_buf));
                Logger::get_instance().error("SSL_connect failed: " + std::string(error_buf) + 
                                           " (SSL error: " + std::to_string(ssl_error) + ")");
                return false;
            }
        }
        
        Logger::get_instance().debug("TLS connection established successfully");
        return true;
    }
    
    std::string generate_websocket_key() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        
        unsigned char random_bytes[16];
        for (int i = 0; i < 16; ++i) {
            random_bytes[i] = static_cast<unsigned char>(dis(gen));
        }
        
        unsigned char encoded[24];
        EVP_EncodeBlock(encoded, random_bytes, 16);
        return std::string(reinterpret_cast<char*>(encoded), 24);
    }
    
    bool perform_websocket_handshake(void* ssl, const ConnectionParams& params) {
        std::ostringstream req;
        req << "GET " << params.path << " HTTP/1.1\r\n"
            << "Host: " << params.host << "\r\n"
            << "Upgrade: websocket\r\n"
            << "Connection: Upgrade\r\n"
            << "Sec-WebSocket-Key: " << generate_websocket_key() << "\r\n"
            << "Sec-WebSocket-Version: 13\r\n";
        
        // Only add Authorization header if token is provided (for direct Socket Mode)
        // Dynamic URL from apps.connections.open uses ticket in URL for auth
        if (!params.token.empty()) {
            req << "Authorization: Bearer " << params.token << "\r\n";
        }
        
        req << "\r\n";
        std::string req_str = req.str();
        
        Logger::get_instance().debug("Sending WebSocket handshake request to " + 
                                     std::string(params.host) + params.path);
        
        if (SSL_write(static_cast<SSL*>(ssl), req_str.c_str(), req_str.length()) <= 0) {
            unsigned long ssl_error = ERR_get_error();
            char error_buf[256];
            ERR_error_string_n(ssl_error, error_buf, sizeof(error_buf));
            Logger::get_instance().error("SSL_write failed: " + std::string(error_buf));
            return false;
        }
        
        // Read full HTTP response (may come in multiple packets)
        std::string resp;
        char buffer[4096];
        bool headers_complete = false;
        const int max_reads = 10;
        int read_count = 0;
        
        while (!headers_complete && read_count < max_reads) {
            // Wait a bit for data to arrive
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            int bytes = SSL_read(static_cast<SSL*>(ssl), buffer, sizeof(buffer) - 1);
            if (bytes <= 0) {
                int ssl_error = SSL_get_error(static_cast<SSL*>(ssl), bytes);
                if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
                    // Wait a bit more and retry
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    read_count++;
                    continue;
                } else {
                    unsigned long err = ERR_get_error();
                    char error_buf[256];
                    ERR_error_string_n(err, error_buf, sizeof(error_buf));
                    Logger::get_instance().error("SSL_read failed: " + std::string(error_buf));
                    break;
                }
            }
            
            buffer[bytes] = '\0';
            resp.append(buffer, bytes);
            
            // Check if we have complete headers (end with \r\n\r\n)
            if (resp.find("\r\n\r\n") != std::string::npos) {
                headers_complete = true;
            }
            
            read_count++;
        }
        
        if (resp.empty()) {
            Logger::get_instance().error("Empty response from server during WebSocket handshake");
            return false;
        }
        
        // Log response for debugging (truncate if too long)
        std::string log_resp = resp;
        if (log_resp.length() > 500) {
            log_resp = log_resp.substr(0, 500) + "... (truncated)";
        }
        Logger::get_instance().debug("WebSocket handshake response: " + log_resp);
        
        // Check for successful handshake (case-insensitive)
        // Server may return "upgrade: websocket" (lowercase) or "Upgrade: websocket"
        std::string resp_lower = resp;
        std::transform(resp_lower.begin(), resp_lower.end(), resp_lower.begin(), ::tolower);
        
        bool success = resp.find("HTTP/1.1 101") != std::string::npos &&
                       resp_lower.find("upgrade: websocket") != std::string::npos;
        
        if (!success) {
            Logger::get_instance().error("WebSocket handshake failed. Response: " + 
                                       resp.substr(0, 200));
        } else {
            Logger::get_instance().info("WebSocket handshake successful!");
        }
        
        return success;
    }
    
    void cleanup_connection(void*& ssl_ctx, void*& ssl, int& socket_fd) {
        // Safe cleanup - check if already cleaned up
        if (ssl) {
            // Try graceful shutdown, but ignore errors if already closed
            // SSL_shutdown can fail if connection is already closed, which is OK
            // We don't check return value because connection might already be closed
            int shutdown_result = SSL_shutdown(static_cast<SSL*>(ssl));
            // Ignore errors - connection might already be closed
            (void)shutdown_result; // Suppress unused variable warning
            SSL_free(static_cast<SSL*>(ssl));
            ssl = nullptr;
        }
        if (socket_fd >= 0) {
            close(socket_fd);
            socket_fd = -1;
        }
        if (ssl_ctx) {
            SSL_CTX_free(static_cast<SSL_CTX*>(ssl_ctx));
            ssl_ctx = nullptr;
        }
    }
}
