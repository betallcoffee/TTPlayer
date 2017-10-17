//
//  TTClock.cpp
//  TTPlayerExample
//
//  Created by liang on 11/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include <math.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/time.h"
#ifdef __cplusplus
};
#endif

#include "TTClock.hpp"

using namespace TT;

/* no AV correction is done if too big error */
static const int AV_NOSYNC_THRESHOLD = 100.0;

Clock::Clock() {
    reset();
}

void Clock::reset() {
    _speed = 1.0;
    _paused = 0;
    setClock(NAN);
}

int64_t Clock::getClock()
{
    if (_paused) {
        return _pts;
    } else {
        int64_t time = av_gettime_relative() / 1000000.0;
        return _ptsDrift + time - (time - _lastUpdated) * (1.0 - _speed);
    }
}

void Clock::setClockAt(int64_t pts, int64_t time)
{
    _pts = pts;
    _lastUpdated = time;
    _ptsDrift = _pts - time;
}

void Clock::setClock(int64_t pts)
{
    int64_t time = av_gettime_relative() / 1000000.0;
    setClockAt(pts, time);
}

void Clock::setClockSpeed(double speed)
{
    setClock(getClock());
    _speed = speed;
}

void Clock::syncClockToSlave(Clock &slave)
{
    int64_t clock = getClock();
    int64_t slaveClock = slave.getClock();
    if (!isnan(slaveClock) && (isnan(clock) || abs(clock - slaveClock) > AV_NOSYNC_THRESHOLD))
        setClock(slaveClock);
}


