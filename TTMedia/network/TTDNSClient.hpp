//
//  TTDNSClient.hpp
//  TTPlayerExample
//
//  Created by liang on 26/11/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTDNSClient_hpp
#define TTDNSClient_hpp

#include <netdb.h>
#include <vector>

#include "TTURL.hpp"

namespace TT {
    class DNSClient {
    public:
        DNSClient();
        ~DNSClient();
        
        static bool resolveHost(std::shared_ptr<URL> url, struct addrinfo **ai);
    };
}

#endif /* TTDNSClient_hpp */
