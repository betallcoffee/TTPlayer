//
//  TTHTTPStream.hpp
//  TTPlayerExample
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTHTTPStream_hpp
#define TTHTTPStream_hpp

#include "TTHTTPClient.hpp"
#include "TTStream.hpp"

namespace TT {
    class HTTPStream : public Stream {
    public:
        HTTPStream();
        ~HTTPStream();
        
        bool open(std::shared_ptr<URL> url, uint64_t offset, int flag) override;
        bool reOpen(std::shared_ptr<URL> url, uint64_t offset) override;
        void close() override;
        
        size_t read(uint8_t *pBuf, size_t size) override;
        size_t readAt(uint8_t *pBuf, size_t size, uint64_t pos) override;
        size_t write(const uint8_t *pBuf, size_t size) override;
        bool seek(uint64_t pos) override;
        
        int64_t length() override;
        int64_t readPos() override;
        int64_t downPos() override;
        int64_t speed() override;
        
    private:
        HTTPClient _client;
    };
}

#endif /* TTHTTPStream_hpp */
