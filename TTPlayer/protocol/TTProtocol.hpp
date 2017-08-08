//
//  TTProtocol.hpp
//  TTPlayerExample
//
//  Created by liang on 4/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTProtocol_hpp
#define TTProtocol_hpp

#include <stdint.h>
#include <unistd.h>

namespace TT {
    class Protocol {
    public:
        Protocol();
        virtual ~Protocol();
        
        virtual bool open(const char *pUrl, uint64_t offset, int flag);
        virtual bool reOpen(const char *pUrl, uint64_t offset);
        virtual void close();
        
        virtual size_t read(uint8_t *pBuf, size_t size);
        virtual size_t readAt(uint8_t *pBuf, size_t size, uint64_t pos);
        virtual size_t write(const uint8_t *pBuf, size_t size);
        
        virtual bool seek(uint64_t pos);
        virtual uint64_t length();
        virtual uint64_t readPos();
        virtual uint64_t downPos();
        virtual int32_t speed();
        
    private:
        int _flag;
        uint64_t _length;
        uint64_t _readPos;
        uint64_t _downPos;
    
    };
}

#endif /* TTProtocol_hpp */
