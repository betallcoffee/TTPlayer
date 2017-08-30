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
#include <functional>
#include <pthread.h>

#include "TTMutex.hpp"

namespace TT {
    template<class T>
    class Queue {
    public:
        Queue(const char *name, int capacity = 0) : _name(name), _capacity(capacity), _isClosed(false), _mutex(PTHREAD_MUTEX_INITIALIZER), _cond(PTHREAD_COND_INITIALIZER){}
        ~Queue() {}
        
        bool empty() {
            Mutex m(&_mutex);
            bool ret = _list.empty();
            return ret;
        }
        
        bool full() {
            Mutex m(&_mutex);
            return _full();
        }
        
        size_t size() {
            Mutex m(&_mutex);
            size_t size = _list.size();
            return size;
        }
        
        T pop() {
            Mutex m(&_mutex);
            if (_list.empty() && !_isClosed) {
                pthread_cond_wait(&_cond, &_mutex);
            }
            
            if (_full()) {
                pthread_cond_broadcast(&_cond);
            }
            
            if (_list.empty()) {
                return static_cast<T>(0);
            }
            
            T elm = _list.front();
            if (elm) {
                _list.pop_front();
            }

            return elm;
        }
        
        void insert(T elm, std::function<bool(T, T)> cmp) {
            Mutex m(&_mutex);
            
            while (_full() && !_isClosed) {
                pthread_cond_wait(&_cond, &_mutex);
            }
            
            if (elm) {
                typename std::list<T>::const_reverse_iterator it = _list.rbegin();
                for (; it != _list.rend(); it++) {
                    if (cmp(*it, elm)) {
                        break;
                    }
                }
                if (it == _list.rend()) {
                    _list.push_front(elm);
                } else if (it == _list.rbegin()) {
                    _list.push_back(elm);
                } else {
                    it--;
                    typename std::list<T>::const_iterator pos = it.base();
                    _list.insert(pos, elm);
                }
            }
            pthread_cond_broadcast(&_cond);
        }
        
        void push(T elm) {
            Mutex m(&_mutex);
            
            while (_full() && !_isClosed) {
                pthread_cond_wait(&_cond, &_mutex);
            }

            if (elm) {
                _list.push_back(elm);
            }
            pthread_cond_broadcast(&_cond);
        }
        
        void clear() {
            Mutex m(&_mutex);
            _list.clear();
            pthread_cond_broadcast(&_cond);
        }
        
        void close() {
            Mutex m(&_mutex);
            
            _isClosed = true;
            _list.clear();
            pthread_cond_broadcast(&_cond);
        }
        
    private:
        bool _full() {
            if (_capacity <= 0) return false;
            
            bool ret = _list.size() >= _capacity ? true : false;
            return ret;
        }
        
    private:
        const char *_name;
        int _capacity;
        bool _isClosed;
        std::list<T> _list;
        
        pthread_mutex_t _mutex;
        pthread_cond_t _cond;
    };
}

#endif /* TTQueue_hpp */
