//
//  TTFFWriter.hpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTFFWriter_hpp
#define TTFFWriter_hpp

#include "TTPacket.hpp"
#include "TTFFMuxer.hpp"
#include "TTFilterFrame.hpp"

namespace TT {
    class FFWriter : public FilterFrame {
    public:
        FFWriter();
        ~FFWriter();
        
        bool start(std::shared_ptr<URL> url, AVCodecParameters *audioCodecParam, AVCodecParameters *videoCodecParam);
        bool finish();
        bool cancel();
        
        void processPacket(std::shared_ptr<Packet> packet);
        
        void processFrame(std::shared_ptr<Frame> frame) override;
        void process(int64_t timestamp) override;
        
    private:
        std::shared_ptr<FFMuxer> _muxer;
        std::shared_ptr<URL> _url;
    };
}

#endif /* TTFFWriter_hpp */
