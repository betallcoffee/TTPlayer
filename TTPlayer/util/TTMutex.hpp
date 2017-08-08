//
//  TTMutex.hpp
//  TTPlayerExample
//
//  Created by liang on 15/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTMutex_hpp
#define TTMutex_hpp

#include <pthread.h>

namespace TT {
    class Mutex {
    public:
        explicit Mutex(pthread_mutex_t *mutex) : _mutex(mutex) { if (_mutex) pthread_mutex_lock(_mutex); }
        ~Mutex() { if (_mutex) pthread_mutex_unlock(_mutex); }
        
    private:
        Mutex(const Mutex &m);
        const Mutex & operator=(const Mutex &m);
        
        pthread_mutex_t *_mutex;
        
    };
}

#endif /* TTMutex_hpp */
