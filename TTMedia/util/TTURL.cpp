//
//  TTURL.cpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include "TTURL.hpp"

using namespace TT;

URL::URL(const std::string &url) {
    _url = url;
    parse();
}

URL::URL(const std::string &&url) {
    _url = url;
    parse();
}

void URL::parse() {
    auto iterBegin = _url.begin();
    auto posScheme = _url.find("://");
    if (posScheme == std::string::npos) {
        return;
    }
    _scheme = std::string(iterBegin, iterBegin + posScheme);
    
    auto posHost = _url.find(":", posScheme + 3);
    if (posHost == std::string::npos) {
        posHost = _url.find("/", posScheme + 3);
        if (posHost == std::string::npos) {
            return;
        }
        _host = std::string(iterBegin + posScheme + 3, iterBegin + posHost);
    } else {
        _host = std::string(iterBegin + posScheme + 3, iterBegin + posHost);
        
        auto posPort = _url.find("/", posHost);
        if (posPort != std::string::npos) {
            _port = std::string(iterBegin + posHost + 1, iterBegin + posPort);
            posHost = posPort;
        }
    }
    
    if (_port.empty()) {
        if ("http" == _scheme) {
            _port = "80";
        }
    }
    
    auto posPath = _url.find("?", posHost + 1);
    if (posPath ==  std::string::npos) {
        _path = std::string(iterBegin + posHost + 1, _url.end());
    } else {
        _path = std::string(iterBegin + posHost + 1, iterBegin + posPath);
        _query = std::string(iterBegin + posPath + 1, _url.end());
    }
    
    auto posExtension = _path.find_last_of(".");
    if (posExtension != std::string::npos) {
        _extension = std::string(_path.begin() + posExtension + 1, _path.end());
    }
    
    auto posLastComponent = _path.find_last_of("/");
    if (posLastComponent != std::string::npos) {
        _lastComponent = std::string(_path.begin() + posLastComponent + 1, _path.end());
        _dir = std::string(_path.begin(), _path.begin() + posLastComponent);
    } else {
        _lastComponent = _path;
    }
}

