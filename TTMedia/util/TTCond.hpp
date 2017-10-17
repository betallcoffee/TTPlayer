//
//  TTCond.hpp
//  TTPlayerExample
//
//  Created by liang on 15/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTCond_hpp
#define TTCond_hpp

#include <assert.h>
#include <pthread.h>

#include "TTMutex.hpp"

namespace TT {
    class Cond {
    public:
        explicit Cond(pthread_mutex_t *mutex) : _cond(PTHREAD_COND_INITIALIZER), _mutex(mutex), _isSignal(false) {
            assert(_mutex);
        }
        ~Cond();
        
        void notify() {
            assert(_mutex);
            Mutex m(_mutex);
            pthread_cond_broadcast(&_cond);
        }
        void wait() {
            assert(_mutex);
            Mutex m(_mutex);
            while (!_isSignal) {
                pthread_cond_wait(&_cond, _mutex);
            }
        }
        
    private:
        Cond(const Cond &c);
        const Cond & operator=(const Cond &c);
        
        pthread_cond_t _cond;
        pthread_mutex_t *_mutex;
        bool _isSignal;
    };
}

#endif /* TTCond_hpp */
