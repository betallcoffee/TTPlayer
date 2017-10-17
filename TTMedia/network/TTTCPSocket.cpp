//
//  TTTCPSocket.cpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <string>

#include "TTTCPSocket.hpp"

using namespace TT;

static const int kValidFD = -1;

TCPSocket::TCPSocket() : _fd(kValidFD) {
    
}

TCPSocket::~TCPSocket() {
    uninit();
}

bool TCPSocket::setNonBlock() {
    if (_fd != kValidFD) {
        int flags = ::fcntl(_fd, F_GETFL, 0);
        if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
            ::close(_fd);
            _fd = kValidFD;
        } else {
            return true;
        }
    }
    return false;
}

bool TCPSocket::init() {
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_fd != kValidFD) {
        return true;
    } else {
        return false;
    }
}

void TCPSocket::uninit() {
    if (_fd != kValidFD) {
        ::close(_fd);
        _fd = kValidFD;
    }
}

bool TCPSocket::setTimeout(int seconds) {
    if (_fd == kValidFD) {
        return false;
    }
    
    struct timeval timeout = {seconds, 0};
    
    /* set receive timeout */
    if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout))) {
        return false;
    }
    
    /* set send timeout*/
    if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout))) {
        return false;
    }
    
    _timeout = seconds;

    return true;
}

bool TCPSocket::connect(const char *ip, uint8_t port) {
    if (ip == NULL) {
        return false;
    }
    
    memset(&_addr, 0, sizeof(struct sockaddr_in));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = ::inet_addr(ip);

    int ret = ::connect(_fd, (struct sockaddr *)&_addr, sizeof(struct sockaddr_in));
    
    return ret == 0;
}

size_t TCPSocket::read(uint8_t *buf, size_t size) {
    if (_fd == kValidFD || buf == NULL || size <= 0) {
        return -1;
    }
    
    size_t ret = ::read(_fd, buf, size);
    return ret;
}

size_t TCPSocket::write(const uint8_t *buf, size_t size) {
    if (_fd == kValidFD || buf == NULL || size <= 0) {
        return -1;
    }
    
    size_t ret = ::write(_fd, buf, size);
    return ret;
}







