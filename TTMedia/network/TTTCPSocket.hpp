//
//  TTTCPSocket.hpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTTCPSocket_hpp
#define TTTCPSocket_hpp

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "TTURL.hpp"

namespace TT {
    class TCPSocket {
    public:
        TCPSocket();
        ~TCPSocket();
        
        bool init();
        void uninit();
        
        bool setTimeout(int seconds);
        
        typedef std::function<bool(void *)> InterruptCallback;
        void setInterruptCallback(InterruptCallback callback) {
            _interruptCB = callback;
        }
        
        
        bool connect(std::shared_ptr<URL> url);
        int read(uint8_t *buf, size_t size);
        int write(const uint8_t *buf, size_t size);
        
    private:
        bool setNonBlock();
        
    private:
        int _fd;
        struct sockaddr_in _addr;
        int _timeout = 0; // 单位秒
        InterruptCallback _interruptCB = nullptr;
        void *_opaque = nullptr;
    };
}

#endif /* TTSocket_hpp */
