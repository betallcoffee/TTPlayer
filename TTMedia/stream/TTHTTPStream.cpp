//
//  TTHTTPStream.cpp
//  TTPlayerExample
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTHTTPStream.hpp"

using namespace TT;

HTTPStream::HTTPStream() {
    
}

HTTPStream::~HTTPStream() {
    
}

bool HTTPStream::open(std::shared_ptr<URL> url, uint64_t offset, int flag) {
    HTTPClient::HeaderMap headers;
    _client.Get(url, headers, std::bind(&HTTPStream::onDataRecived, this, std::placeholders::_1), nullptr);
    return true;
}

bool HTTPStream::reOpen(std::shared_ptr<URL> url, uint64_t offset) {
    return false;
}

void HTTPStream::close() {
    
}

size_t HTTPStream::read(uint8_t *pBuf, size_t size) {
    if (nullptr == pBuf || size <= 0) {
        return 0;
    }
    
    Mutex m(&_mutex);
    if (_buffer.empty()) {
        return 0;
    }
    
    const char *begin = _buffer.beginRead();
    if (begin) {
        size_t readSize = _buffer.readableBytes();
        size = size <= readSize ? size : readSize;
        memcpy(pBuf, begin, size);
        return size;
    }
    
    return 0;
}

size_t HTTPStream::readAt(uint8_t *pBuf, size_t size, uint64_t pos) {
    return 0;
}

size_t HTTPStream::write(const uint8_t *pBuf, size_t size) {
    return 0;
}

bool HTTPStream::seek(uint64_t pos) {
    return false;
}

int64_t HTTPStream::length() {
    return 0;
}

int64_t HTTPStream::readPos() {
    return 0;
}

int64_t HTTPStream::downPos() {
    return 0;
}

int64_t HTTPStream::speed() {
    return 0;
}

void HTTPStream::onDataRecived(Buffer &data) {
    Mutex m(&_mutex);
    _buffer.appendBuffer(data);
}





