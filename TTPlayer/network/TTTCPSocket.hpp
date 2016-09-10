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

namespace TT {
    class TCPSocket {
    public:
        TCPSocket();
        ~TCPSocket();
        
        bool setNonBlock();
        bool setTimeout(int seconds);
        bool connect(const char *ip, uint8_t port);
        int read(uint8_t *buf, size_t size);
        int write(const uint8_t *buf, size_t size);
        
    private:
        int _fd;
        struct sockaddr_in _addr;
    };
}

#endif /* TTSocket_hpp */
