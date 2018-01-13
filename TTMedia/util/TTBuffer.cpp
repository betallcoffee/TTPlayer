//
//  TTBuffer.cpp
//  TTPlayerExample
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include <string>

#include "TTBuffer.hpp"

using namespace TT;

const char Buffer::kCRLF[] = { "\r\n" };

Buffer::Buffer()
: readIndex_(0),
writeIndex_(0)
{
}

Buffer::~Buffer()
{
}

size_t Buffer::append(const char *data, size_t n)
{
    ensureWriteable(n);
    std::copy(data, data + n, beginWrite());
    writeIndex_ += n;
    return n;
}

size_t Buffer::appendBuffer(Buffer &buffer)
{
    char *data = buffer.beginRead();
    size_t size = buffer.readableBytes();
    return append(data, size);
}

const char *Buffer::find(const char *sub) {
    size_t size = strlen(sub);
    if (size > readableBytes()) {
        return NULL;
    } else if (strncmp(sub, beginRead(), size) == 0) {
        return beginRead();
    }
    
    const char *res = std::search(beginRead(), beginWrite(), sub, sub + size);
    return res == beginWrite() ? NULL : res;
}

bool Buffer::beginCRLF() {
    if (readableBytes() > 2) {
        char *begin = beginRead();
        if (strncmp(begin, kCRLF, 2) == 0) {
            return true;
        }
    }
    return false;
}

const char *Buffer::findCRLF()
{
    return findCRLF(beginRead());
}

const char *Buffer::findCRLF(char *start)
{
    return find(kCRLF);
}

void Buffer::skipSpace() {
    while (readableBytes()) {
        if (*beginRead() == ' ') {
            retrieve(1);
        } else {
            break;
        }
    }
}

void Buffer::skipCRLF() {
    const char *crlf = findCRLF();
    int n = static_cast<int>(crlf - beginRead());
    n += strlen(kCRLF);
    retrieve(n);
}

bool Buffer::getLine(std::string &line) {
    const char *begin = beginRead();
    const char *end = findCRLF();
    if (end == NULL) return false;
    line.assign(begin, end - begin + 2);
    retrieve(line.size());
    return true;
}

int Buffer::readInt() {
    int n = 0;
    while (readableBytes()) {
        char ch = *beginRead();
        if (ch >= '0' && ch <= '9') {
            n *= 10;
            n += ch - '0';
            retrieve(1);
        } else {
            break;
        }
    }
    return n;
}

std::string Buffer::toString() {
    return std::string(beginRead(), readableBytes());
}

void Buffer::ensureWriteable(size_t n) {
    if (n > writeableBytes()) {
        expend(n);
    }
}

void Buffer::expend(size_t size) {
    if (size > writeableBytes() + prependableBytes()) {
        buffer_.resize(writeIndex_ + size);
    } else {
        size_t readable = readableBytes();
        std::copy(beginRead(), beginWrite(), begin());
        readIndex_ = 0;
        writeIndex_ = readIndex_+ readable;
    }
}

