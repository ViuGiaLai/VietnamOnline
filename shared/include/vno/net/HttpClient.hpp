#pragma once
#include <string>
#include <vector>
#include <utility>

namespace vno::net {

struct HttpResponse {
    int status = 0;
    std::string body;
    bool ok() const { return status >= 200 && status < 300; }
};

struct HttpHeader {
    std::string name;
    std::string value;
};

/// Minimal HTTP client (WinHTTP on Windows). Used for Supabase REST/Auth.
HttpResponse httpRequest(
    const std::string& method,
    const std::string& url,
    const std::vector<HttpHeader>& headers,
    const std::string& body = "");

} // namespace vno::net
