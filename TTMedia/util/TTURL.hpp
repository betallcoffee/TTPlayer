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
        ~URL() {};
        
        const char *cStr() { return _url.c_str(); }
        std::string &scheme() { return _scheme; }
        std::string &host() { return _host; }
        std::string &path() { return _path; }
        std::string &extension() { return _extension; }
        std::string &query() { return _query; }
        
    private:
        std::string _url;
        std::string _scheme;
        std::string _host;
        std::string _path;
        std::string _extension;
        std::string _query;
    };
}

#endif /* TTURL_hpp */
