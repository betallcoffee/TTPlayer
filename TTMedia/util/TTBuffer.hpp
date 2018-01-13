//
//  TTBuffer.hpp
//  TTPlayerExample
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTBuffer_hpp
#define TTBuffer_hpp

#include <vector>

namespace TT {
    class Buffer {
    public:
        Buffer();
        ~Buffer();
        
        char *beginRead() { return begin() + readIndex_; }
        char *beginWrite() { return begin() + writeIndex_; }
        size_t readableBytes() { return writeIndex_ - readIndex_; }
        size_t writeableBytes() { return buffer_.size() - writeIndex_; }
        size_t prependableBytes() { return readIndex_; }
        bool empty() { return readIndex_ == writeIndex_; }
        
        size_t append(const char *data, size_t n);
        size_t appendBuffer(Buffer &buffer);
        void reviseWriteable(size_t n) { writeIndex_ += n; }
        
        const char *find(const char *sub);
        bool beginCRLF();
        const char *findCRLF();
        const char *findCRLF(char *start);
        
        void ensureWriteable(size_t n);
        /**
         * Remove the n byte data from buffer.
         */
        void retrieve(size_t n) { readIndex_ += n; }
        void clear() { readIndex_ = writeIndex_ = 0; }
        void skipSpace();
        void skipCRLF();
        
        /**
         * Read a line data, and remove the data from buffer.
         */
        bool getLine(std::string &line);
        
        int readInt();
        
        std::string toString();
        
    private:
        static const char kCRLF[];
        
        char *begin() { return &*buffer_.begin(); }
        void expend(size_t size);
        
        std::vector<char> buffer_;
        size_t readIndex_ = 0;
        size_t writeIndex_ = 0;
    };
}

#endif /* TTBuffer_hpp */
