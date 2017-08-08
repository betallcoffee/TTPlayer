//
//  TTQueue.hpp
//  TTPlayerExample
//
//  Created by liang on 8/19/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTQueue_hpp
#define TTQueue_hpp

#include <list>
#include <pthread.h>

#include "TTMutex.hpp"

using namespace std;

namespace TT {
    template<class T>
    class Queue {
    public:
        Queue(const char *name) : _name(name), _isClosed(false), _mutex(PTHREAD_MUTEX_INITIALIZER), _cond(PTHREAD_COND_INITIALIZER){}
        ~Queue() {}
        
        bool empty() {
            Mutex m(&_mutex);
            bool ret = _list.empty();
            return ret;
        }
        
        size_t size() {
            Mutex m(&_mutex);
            size_t size = _list.size();
            return size;
        }
        
        T pop() {
            Mutex m(&_mutex);
            while (_list.empty() && !_isClosed) {
                pthread_cond_wait(&_cond, &_mutex);
            }
            T elm = _list.front();
            if (elm) {
                _list.pop_front();
            }

            return elm;
        }
        
        void push(T elm) {
            Mutex m(&_mutex);

            if (elm) {
                _list.push_back(elm);
            }
            pthread_cond_broadcast(&_cond);
        }
        
        void close() {
            Mutex m(&_mutex);
            
            _isClosed = true;
            _list.clear();
            pthread_cond_broadcast(&_cond);
        }
        
    private:
        const char *_name;
        bool _isClosed;
        list<T> _list;
        
        pthread_mutex_t _mutex;
        pthread_cond_t _cond;
    };
}

#endif /* TTQueue_hpp */
