//
//  TTURL.hpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTURL_hpp
#define TTURL_hpp

#include <stdio.h>
#include <string>

namespace TT {
    class URL {
    public:
        URL(const std::string &&url);
        URL(const std::string &url);
        ~URL() {};
        
        const char *cStr() const { return _url.c_str(); }
        const std::string &string() const { return _url; }
        const std::string &scheme() const { return _scheme; }
        const std::string &host() const { return _host; }
        const std::string &port() const { return _port; }
        const std::string &path() const { return _path; }
        const std::string &extension() const { return _extension; }
        const std::string &query() const { return _query; }
        const std::string &dir() const { return _dir; }
        const std::string &lastComponent() const { return _lastComponent; }
        
    private:
        void parse();
        
        std::string _url = "";
        std::string _scheme = "";
        std::string _host = "";
        std::string _port = "";
        std::string _path = "";
        std::string _extension = "";
        std::string _query = "";
        std::string _dir = "";
        std::string _lastComponent = "";
    };
}

#endif /* TTURL_hpp */
