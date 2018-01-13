//
//  TTEdit.hpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTEdit_hpp
#define TTEdit_hpp

#include <pthread.h>
#include <vector>

#include "TTQueue.hpp"
#include "TTCond.hpp"
#include "TTURL.hpp"
#include "TTFrame.hpp"
#include "TTStream.hpp"
#include "TTVideoCodec.hpp"

namespace TT {
    class Edit {
    public:
        Edit();
        ~Edit();
        
        void open(std::shared_ptr<URL> url);
        void close();
        
        int previewCount();
        std::shared_ptr<Frame> preview(int index);
        
    private:
        static void *inputThreadEntry(void *arg);
        void inputLoop();
        
    private:
        std::shared_ptr<URL> _url;
        std::shared_ptr<Stream> _stream;
        std::vector<std::shared_ptr<Frame>> _previews;
        
        std::shared_ptr<VideoCodec> _videoCodec;
        
        pthread_t _inputThread;
        pthread_cond_t _inputCond;
        pthread_mutex_t _inputMutex;
        
    };
}

#endif /* TTEdit_hpp */
