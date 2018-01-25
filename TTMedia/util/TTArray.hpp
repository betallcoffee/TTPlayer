//
//  TTArray.hpp
//  TTPlayerExample
//
//  Created by liang on 14/1/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTArray_hpp
#define TTArray_hpp

#include <vector>
#include <pthread.h>

#include "TTMutex.hpp"

namespace TT {
    template<class T>
    class Array {
    public:
        Array(const char *name, int capacity = 0) : _name(name), _capacity(capacity), _isClosed(false), _mutex(PTHREAD_MUTEX_INITIALIZER), _cond(PTHREAD_COND_INITIALIZER){}
        ~Array() {}
        
        bool empty() {
            Mutex m(&_mutex);
            bool ret = _array.empty();
            return ret;
        }
        
        bool full() {
            Mutex m(&_mutex);
            return _full();
        }
        
        size_t size() {
            Mutex m(&_mutex);
            size_t size = _array.size();
            return size;
        }
        
        T operator[] (int index) {
            Mutex m(&_mutex);
            return _array[index];
        }
        
        void insert(T &elm, int index) {
            Mutex m(&_mutex);
            if (!_full()) {
                _array[index] = elm;
            }
        }
        
        void erase(int index) {
            Mutex m(&_mutex);
            _array.erase(index);
        }
        
    private:
        bool _full() {
            if (_capacity <= 0) return false;
            
            bool ret = _array.size() >= _capacity ? true : false;
            return ret;
        }

    private:
        const char *_name;
        int _capacity;
        bool _isClosed;
        std::vector<T> _array;
        
        pthread_mutex_t _mutex;
        pthread_cond_t _cond;
    };
}

#endif /* TTArray_hpp */
