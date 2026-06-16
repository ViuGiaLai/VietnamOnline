#include "vno/net/HttpClient.hpp"
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#endif

namespace vno::net {

namespace {

struct ParsedUrl {
    bool https = true;
    std::wstring host;
    std::wstring path;
    INTERNET_PORT port = 443;
};

std::wstring toWide(const std::string& s) {
    if (s.empty()) return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring out(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, out.data(), len);
    return out;
}

bool parseUrl(const std::string& url, ParsedUrl& out) {
    std::string rest = url;
    out.https = true;
    out.port = 443;
    if (rest.rfind("https://", 0) == 0) {
        rest = rest.substr(8);
    } else if (rest.rfind("http://", 0) == 0) {
        out.https = false;
        out.port = 80;
        rest = rest.substr(7);
    } else {
        return false;
    }
    auto slash = rest.find('/');
    std::string host_port = slash == std::string::npos ? rest : rest.substr(0, slash);
    std::string path = slash == std::string::npos ? "/" : rest.substr(slash);
    auto colon = host_port.find(':');
    if (colon != std::string::npos) {
        out.port = static_cast<INTERNET_PORT>(std::stoi(host_port.substr(colon + 1)));
        host_port = host_port.substr(0, colon);
    }
    out.host = toWide(host_port);
    out.path = toWide(path.empty() ? "/" : path);
    return true;
}

} // namespace

HttpResponse httpRequest(
    const std::string& method,
    const std::string& url,
    const std::vector<HttpHeader>& headers,
    const std::string& body) {
    HttpResponse resp;
#ifndef _WIN32
    resp.status = 0;
    resp.body = "{\"error\":\"HttpClient only implemented on Windows\"}";
    return resp;
#else
    ParsedUrl parsed;
    if (!parseUrl(url, parsed)) return resp;

    HINTERNET session = WinHttpOpen(L"VietnamOnline/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return resp;

    HINTERNET connect = WinHttpConnect(session, parsed.host.c_str(), parsed.port, 0);
    if (!connect) {
        WinHttpCloseHandle(session);
        return resp;
    }

    std::wstring wmethod = toWide(method);
    DWORD flags = parsed.https ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET request = WinHttpOpenRequest(connect, wmethod.c_str(), parsed.path.c_str(),
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!request) {
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return resp;
    }

    std::wstring header_block;
    for (const auto& h : headers) {
        header_block += toWide(h.name + ": " + h.value + "\r\n");
    }
    if (!header_block.empty()) {
        WinHttpAddRequestHeaders(request, header_block.c_str(), static_cast<DWORD>(-1L),
            WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
    }

    BOOL sent = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        body.empty() ? WINHTTP_NO_REQUEST_DATA : const_cast<char*>(body.data()),
        static_cast<DWORD>(body.size()), static_cast<DWORD>(body.size()), 0);
    if (!sent || !WinHttpReceiveResponse(request, nullptr)) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return resp;
    }

    DWORD status_code = 0;
    DWORD size = sizeof(status_code);
    WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status_code, &size, WINHTTP_NO_HEADER_INDEX);
    resp.status = static_cast<int>(status_code);

    DWORD available = 0;
    do {
        if (!WinHttpQueryDataAvailable(request, &available) || available == 0) break;
        std::string chunk(available, '\0');
        DWORD read = 0;
        if (!WinHttpReadData(request, chunk.data(), available, &read)) break;
        chunk.resize(read);
        resp.body += chunk;
    } while (available > 0);

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return resp;
#endif
}

} // namespace vno::net
