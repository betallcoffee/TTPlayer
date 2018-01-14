//
//  TTHTTPClient.hpp
//  TTPlayerExample
//
//  Created by liang on 26/11/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTHTTPClient_hpp
#define TTHTTPClient_hpp

#include <string>
#include <map>
#include <vector>

#include "TTBuffer.hpp"
#include "TTURL.hpp"
#include "TTTCPSocket.hpp"

namespace TT {
    class HTTPClient {
    public:
        HTTPClient();
        ~HTTPClient();
        
        typedef std::map<std::string, std::string> HeaderMap;
        typedef std::function<void(Buffer &dataxs)> DataRecivedCallback;
        typedef std::function<void()> ErrorCallback;
        void Get(std::shared_ptr<URL> url, const HeaderMap &headers, DataRecivedCallback dataCallback, ErrorCallback errorCallback);
        
        int GetStatusCode() { return _statusCode; }
        
    private:
        void skipToNext();
        
        bool parserFirstLine();
        bool parserHeaders();
        
        std::shared_ptr<TCPSocket> _socket;
        
        enum {kMaxBufferSize = 10 * 1024};
        Buffer _buffer;
        
        enum ParseStatus {
            kParseFirstLine,
            kParseHeader,
            kParseBody,
            kParseEnd,
        };
        ParseStatus _parseStatus;
        
        std::string _httpVer;
        int _statusCode;
        std::string _phase;
        
        bool _isClose;
        bool _isChunked;
        std::string _contentType;
        int64_t _contentLength;
        
        
    };
}

#endif /* TTHTTPClient_hpp */
