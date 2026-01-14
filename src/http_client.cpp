#include "http_client.hpp"
#include "include/httplib.h"
#include "logger.hpp"
#include <iostream>
#include <sstream>
#include <regex>

HttpClient::HttpClient() : timeout_seconds_(30), client_(nullptr) {
}

HttpClient::~HttpClient() {
    client_.reset();
}

std::pair<std::string, std::string> HttpClient::parse_url(const std::string& url) {
    std::regex url_regex(R"(https?://([^/:]+)(?::(\d+))?(.*))");
    std::smatch match;
    
    if (std::regex_match(url, match, url_regex)) {
        std::string host = match[1].str();
        std::string port = match[2].str();
        std::string path = match[3].str();
        
        // If no port specified and URL is https, default to 443
        if (port.empty() && url.find("https://") == 0) {
            port = "443";
        }
        
        // If port is specified, append it to host
        if (!port.empty()) {
            host += ":" + port;
        }
        
        return {host, path.empty() ? "/" : path};
    }
    return {"", ""};
}

void HttpClient::apply_headers() {
    if (!client_) return;
    httplib::Headers headers_map;
    for (const auto& [key, value] : headers_) {
        headers_map.emplace(key, value);
    }
    client_->set_default_headers(headers_map);
}

bool HttpClient::get(const std::string& url, std::string& response) {
    auto [host, path] = parse_url(url);
    if (host.empty() || path.empty()) {
        return false;
    }
    
    // Use full URL scheme for httplib to auto-detect SSL
    // httplib::Client with scheme_host_port automatically creates SSLClient for https://
    // Requires CPPHTTPLIB_OPENSSL_SUPPORT macro (defined in Makefile)
    size_t scheme_end = url.find("://");
    if (scheme_end == std::string::npos) {
        return false;
    }
    
    size_t path_start = url.find('/', scheme_end + 3);
    std::string scheme_host_port;
    if (path_start != std::string::npos) {
        scheme_host_port = url.substr(0, path_start);
    } else {
        // No path found, use entire URL as scheme_host_port
        scheme_host_port = url;
    }
    
    // If no port specified, add default port
    if (scheme_host_port.find(":443") == std::string::npos && 
        scheme_host_port.find(":80") == std::string::npos) {
        if (url.find("https://") == 0) {
            scheme_host_port += ":443";
        } else if (url.find("http://") == 0) {
            scheme_host_port += ":80";
        }
    }
    
    client_ = std::make_unique<httplib::Client>(scheme_host_port);
    client_->set_read_timeout(timeout_seconds_);
    client_->set_connection_timeout(timeout_seconds_);
    client_->set_follow_location(true); // Follow redirects automatically
    apply_headers();
    
    auto res = client_->Get(path.c_str());
    if (res) {
        response = res->body;
        if (res->status == 200) {
            return true;
        }
        // For non-200 status, include status code in response for debugging
        if (response.empty()) {
            response = "HTTP " + std::to_string(res->status);
        }
        return false;
    }
    // If res is null, there was a connection error
    response = "Connection failed";
    return false;
}

bool HttpClient::post(const std::string& url, const std::string& data, 
                     std::string& response) {
    auto [host, path] = parse_url(url);
    if (host.empty() || path.empty()) {
        Logger::get_instance().error("Invalid URL in POST: " + url);
        return false;
    }
    
    Logger::get_instance().debug("POST request: host=" + host + ", path=" + path);
    
    // Use full URL scheme for httplib to auto-detect SSL
    // httplib::Client with scheme_host_port automatically creates SSLClient for https://
    // Extract scheme and host:port from URL
    size_t scheme_end = url.find("://");
    if (scheme_end == std::string::npos) {
        Logger::get_instance().error("Invalid URL scheme: " + url);
        response = "Invalid URL";
        return false;
    }
    
    size_t path_start = url.find('/', scheme_end + 3);
    std::string scheme_host_port;
    if (path_start != std::string::npos) {
        scheme_host_port = url.substr(0, path_start);
    } else {
        // No path found, use entire URL as scheme_host_port
        scheme_host_port = url;
    }
    
    // If no port specified, add default port
    if (scheme_host_port.find(":443") == std::string::npos && 
        scheme_host_port.find(":80") == std::string::npos &&
        scheme_host_port.find("://") != std::string::npos) {
        if (url.find("https://") == 0) {
            scheme_host_port += ":443";
        } else if (url.find("http://") == 0) {
            scheme_host_port += ":80";
        }
    }
    
    Logger::get_instance().debug("Creating httplib client with scheme: " + scheme_host_port);
    
    // httplib Client constructor with scheme_host_port automatically creates SSLClient for https://
    // Requires CPPHTTPLIB_OPENSSL_SUPPORT macro to be defined (now in Makefile)
    client_ = std::make_unique<httplib::Client>(scheme_host_port);
    client_->set_read_timeout(timeout_seconds_);
    client_->set_connection_timeout(timeout_seconds_);
    client_->set_follow_location(true); // Follow redirects automatically
    apply_headers();
    
    // Determine Content-Type from headers, default to application/json
    std::string content_type = "application/json";
    auto it = headers_.find("Content-Type");
    if (it != headers_.end()) {
        content_type = it->second;
    }
    
    httplib::Result res;
    
    // For form-urlencoded, use proper httplib method
    if (content_type == "application/x-www-form-urlencoded") {
        // Convert headers to httplib format, but exclude Content-Type
        // httplib will automatically set Content-Type to application/x-www-form-urlencoded
        httplib::Headers headers_map;
        for (const auto& [key, value] : headers_) {
            // Don't include Content-Type - httplib sets it automatically for Params
            if (key != "Content-Type") {
                headers_map.emplace(key, value);
            }
        }
        
        // For form-urlencoded, use Params (even if empty)
        // httplib will automatically set Content-Type to application/x-www-form-urlencoded
        httplib::Params params;
        // If data is not empty, parse it as form data
        if (!data.empty()) {
            // Simple parsing: key=value&key2=value2 format
            std::istringstream iss(data);
            std::string pair;
            while (std::getline(iss, pair, '&')) {
                size_t eq_pos = pair.find('=');
                if (eq_pos != std::string::npos) {
                    std::string key = pair.substr(0, eq_pos);
                    std::string value = pair.substr(eq_pos + 1);
                    params.emplace(key, value);
                }
            }
        }
        Logger::get_instance().debug("Sending POST with form-urlencoded, params count: " + 
                                      std::to_string(params.size()));
        res = client_->Post(path.c_str(), headers_map, params);
    } else {
        // For JSON or other content types, use standard Post
        Logger::get_instance().debug("Sending POST with content-type: " + content_type + 
                                      ", body size: " + std::to_string(data.size()));
        res = client_->Post(path.c_str(), data, content_type.c_str());
    }
    
    if (res) {
        Logger::get_instance().debug("POST response received, status: " + 
                                      std::to_string(res->status));
        response = res->body;
        // Log status code for debugging
        if (res->status != 200) {
            Logger::get_instance().debug("HTTP POST status: " + std::to_string(res->status));
        }
        // Return true for 200 OK, but also capture response for error handling
        if (res->status == 200) {
            return true;
        }
        // For non-200 status, include status code in response for debugging
        if (response.empty()) {
            response = "HTTP " + std::to_string(res->status);
        }
        return false;
    }
    // If res is null, there was a connection error
    Logger::get_instance().error("POST connection failed for: " + url);
    Logger::get_instance().error("Host: " + host + ", Path: " + path);
    response = "Connection failed";
    return false;
}

void HttpClient::set_header(const std::string& key, const std::string& value) {
    headers_[key] = value;
}

void HttpClient::set_timeout(int seconds) {
    timeout_seconds_ = seconds;
}
