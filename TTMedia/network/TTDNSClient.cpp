//
//  TTDNSClient.cpp
//  TTPlayerExample
//
//  Created by liang on 26/11/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTDNSClient.hpp"

using namespace TT;

DNSClient::DNSClient() {
    
}

DNSClient::~DNSClient() {
    
}

bool DNSClient::resolveHost(std::shared_ptr<URL> url, struct addrinfo **ai) {
    if (ai != nullptr) {
        struct addrinfo hint;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_UNSPEC;
        hint.ai_socktype = SOCK_STREAM;
        
        const char *host = url->host().c_str();
        const char *port = url->port().c_str();
        if (host != nullptr) {
            int ret = getaddrinfo(host, port, &hint, ai);
            if (ret) {
                return false;
            }
            return true;
        }
    }
    return false;
}
