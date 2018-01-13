//
//  TTTCPSocket.cpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <string>
#include <stdlib.h>

#include "easylogging++.h"

#include "TTTCPSocket.hpp"
#include "TTDNSClient.hpp"

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

bool TCPSocket::connect(std::shared_ptr<URL> url) {
    struct addrinfo *ai;
    if (!DNSClient::resolveHost(url, &ai)) {
        return false;
    }
    
    int ret = 0;
    uint16_t port = std::stoi(url->port().c_str());
    struct addrinfo *curAi = ai;
    while (curAi) {
        LOG(INFO) << "Try addrinfo ai_flags:" << curAi->ai_flags
        << " ai_family:" << curAi->ai_family
        << " ai_socktype:" << curAi->ai_socktype
        << " ai_protocol:" << curAi->ai_protocol;
        
        if (curAi->ai_family == AF_INET6) {
            struct sockaddr_in6 *in6 = reinterpret_cast<struct sockaddr_in6 *>(curAi->ai_addr);
            in6->sin6_port = htons(port);
        } else {
            struct sockaddr_in *addr = (struct sockaddr_in *)curAi->ai_addr;
            char *ip = inet_ntoa(addr->sin_addr);
            uint16_t port = ntohs(addr->sin_port);
            LOG(INFO) << "Try connect ip:" << ip << " port:" << port;
        }
        
        ret = ::connect(_fd, curAi->ai_addr, sizeof(struct sockaddr));
        
        if (ret == 0) {
            return true;
        } else {
            curAi = curAi->ai_next;
            LOG(WARNING) << "Test next addrinfo";
        }
    }
    
    LOG(ERROR) << "Connect failed" << url->string();
    return false;
}

int TCPSocket::read(uint8_t *buf, size_t size) {
    if (_fd == kValidFD || buf == NULL || size <= 0) {
        return -1;
    }
    
    size_t ret = ::read(_fd, buf, size);
    return ret;
}

int TCPSocket::write(const uint8_t *buf, size_t size) {
    if (_fd == kValidFD || buf == NULL || size <= 0) {
        return -1;
    }
    
    size_t sendSize = 0;
    while (true) {
        if (_interruptCB) {
            if (_interruptCB(_opaque)) {
                break;
            }
        }
        
        if (sendSize >= size) {
            break;
        }
        
        ssize_t ret = ::write(_fd, buf + sendSize, size - sendSize);
        if (ret >= 0) {
            sendSize += ret;
        } else if (errno == EINTR ||
                   errno == EAGAIN ||
                   errno == EWOULDBLOCK) {
            continue;
        } else {
            break;
        }
    }

    return sendSize;
}







