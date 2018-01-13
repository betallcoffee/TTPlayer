//
//  TTHTTPClient.cpp
//  TTPlayerExample
//
//  Created by liang on 26/11/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"
#include "TTHTTPClient.hpp"

using namespace TT;

static const char *kCRLF = "\r\n";
static const char kColon = ':';
static const char *kHTTP = "HTTP/1.";

HTTPClient::HTTPClient() {
    
}

HTTPClient::~HTTPClient() {
    
}
//GET /%s HTTP/1.1\r\n%sHost: %s:%d\r\nRange: bytes=%ld-\r\nConnection: keep-alive\r\n\r\n
void HTTPClient::Get(std::shared_ptr<URL> url,
                     const HeaderMap &headers,
                     DataRecivedCallback dataCallback,
                     ErrorCallback errorCallback) {
    std::string request = "GET ";
    request += url->path();
    request += " HTTP/1.1";
    request += kCRLF;
    
    request += "Connection: Close";
    request += kCRLF;
    
    request += "Host: ";
    request += url->host();
    request += kCRLF;
    
    request += "User-Agent: TTPlayer";
    request += kCRLF;
    
    request += kCRLF;
    
    _socket = std::make_shared<TCPSocket>();
    if (!_socket->init()){
        LOG(ERROR) << "Init socket failed:" << url->string();
        if (errorCallback) {
            errorCallback();
        }
        return;
    }
    
    if (_socket->connect(url)) {
        size_t writeSize = _socket->write(reinterpret_cast<const uint8_t *>(request.c_str()), request.size());
        _parseStatus = kParseFirstLine;
        if (writeSize == request.size()) {
            do {
                _buffer.ensureWriteable(kMaxBufferSize);
                uint8_t *buf = reinterpret_cast<uint8_t*>(_buffer.beginRead());
                size_t size = _buffer.writeableBytes();
                int ret = _socket->read(buf, size);
                if (ret < 0) {
                    break;
                } else {
                    _buffer.reviseWriteable(ret);
                }
                    
                if (kParseFirstLine == _parseStatus) {
                    const char *lineEnd = _buffer.findCRLF();
                    if (lineEnd == nullptr) {
                        continue;
                    }
                    
                    if (!parserFirstLine()) {
                        break;
                    }
                    _parseStatus = kParseHeader;
                } else if (kParseHeader == _parseStatus) {
                    const char *lineEnd = _buffer.findCRLF();
                    if (lineEnd == nullptr) {
                        continue;
                    }
                    
                    if (!parserHeaders()) {
                        break;
                    }
                } else if (kParseBody == _parseStatus) {
                    if (dataCallback) {
                        dataCallback();
                    }
                }
            } while (true);
            
            if (errorCallback) {
                errorCallback();
            }
        }
    }
}

void HTTPClient::skipToNext() {
    const char *begin = _buffer.beginRead();
    while (*begin == ' ') {
        _buffer.retrieve(1);
    }
}

bool HTTPClient::parserFirstLine() {
    const char *begin = _buffer.beginRead();
    size_t len = std::strlen(kHTTP);
    if (std::strncmp(begin, kHTTP, len) == 0) {
        _httpVer.assign(begin + len - 2, 3);
        const char *space = _buffer.find(" ");
        if (space) {
            _buffer.retrieve(space - begin);
        } else {
            return false;
        }
        
        _buffer.skipSpace();
        _statusCode = _buffer.readInt();
        
        _buffer.skipSpace();
        _phase.assign(_buffer.beginRead(), _buffer.findCRLF() - _buffer.beginRead());
        
        _buffer.skipCRLF();
        return true;
    }

    return false;
}

bool HTTPClient::parserHeaders() {
    if (_buffer.beginCRLF()) {
        _buffer.skipCRLF();
        _parseStatus = kParseBody;
        return true;
    } else {
        const char *lineBegin = _buffer.beginRead();
        const char *lineEnd = _buffer.findCRLF();
        const char *colonPos = std::find(lineBegin, lineEnd, kColon);
        if (lineBegin == colonPos) {
            return false;
        }
        std::string key(_buffer.beginRead(), colonPos - _buffer.beginRead());
        _buffer.retrieve(key.size() + 1);
        _buffer.skipSpace();
        
        std::string value(_buffer.beginRead(), lineEnd - _buffer.beginRead());
        _buffer.skipCRLF();
        
        
        if (key == "Connection") {
            if (value == "close") {
                _isClose = true;
            }
        } else if (key == "Transfer-Encoding") {
            if (value == "chunked") {
                _isChunked = true;
            } else {
                _isChunked = false;
            }
        } else if (key == "Content-Type") {
            _contentType = value;
        } else if (key == "Content-Length") {
            _contentLength = std::stol(value);
        }
        
        return true;
    }
    return false;
}
