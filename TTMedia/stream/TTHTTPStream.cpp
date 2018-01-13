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
    _client.Get(url, headers, nullptr, nullptr);
    return true;
}

bool HTTPStream::reOpen(std::shared_ptr<URL> url, uint64_t offset) {
    return false;
}

void HTTPStream::close() {
    
}

size_t HTTPStream::read(uint8_t *pBuf, size_t size) {
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





