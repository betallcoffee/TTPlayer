//
//  TTStream.hpp
//  TTPlayerExample
//
//  Created by liang on 4/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTStream_hpp
#define TTStream_hpp

#include <stdint.h>
#include <unistd.h>

#include "TTURL.hpp"

namespace TT {
    class Stream {
    public:
        Stream();
        virtual ~Stream();
        
        virtual bool open(std::shared_ptr<URL>, uint64_t offset, int flag) = 0;
        virtual bool reOpen(std::shared_ptr<URL>, uint64_t offset) = 0;
        virtual void close() = 0;
        
        virtual size_t read(uint8_t *pBuf, size_t size) = 0;
        virtual size_t readAt(uint8_t *pBuf, size_t size, uint64_t pos) = 0;
        virtual size_t write(const uint8_t *pBuf, size_t size) = 0;
        virtual bool seek(uint64_t pos) = 0;
        
        virtual int64_t length() = 0;
        virtual int64_t readPos() = 0;
        virtual int64_t downPos() = 0;
        virtual int64_t speed() = 0;
        
    private:
        int _flag;
        uint64_t _length;
        uint64_t _readPos;
        uint64_t _downPos;
    
    };
}

#endif /* TTStream_hpp */
