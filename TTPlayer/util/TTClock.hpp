//
//  TTClock.hpp
//  TTPlayerExample
//
//  Created by liang on 11/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTClock_hpp
#define TTClock_hpp

#include <stdio.h>

namespace TT {
    typedef enum {
        AV_SYNC_AUDIO_MASTER, /* default choice */
        AV_SYNC_VIDEO_MASTER,
        AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
    }AVSyncClock;
    
    class Clock {
    public:
        Clock();
        ~Clock() {}
        
        void reset();
        int64_t getClock();
        void setClockAt(int64_t pts, int64_t time);
        void setClock(int64_t pts);
        void setClockSpeed(double speed);
        void syncClockToSlave(Clock &slave);
        
    private:
        int64_t _pts;           /* clock base */
        int64_t _ptsDrift;     /* clock base minus time at which we updated the clock */
        int64_t _lastUpdated;
        double _speed;
        int _paused;
    };
}

#endif /* TTClock_hpp */
