#ifndef CRPC_TIMER_H_
#define CRPC_TIMER_H_

#include <sys/time.h>
#include <sys/timerfd.h>
#include <map>
#include <list>
#include <fcntl.h>
#include "utills.h"
#include "crpc_log.h"
#include "common_header.h"
namespace crpc
{

struct TimeObject;

class CRpcTimer{
public:

    CRpcTimer():_timer_fd(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)), _cur_timer(get_timestamps_ms())
    {
    }

    int fd() const
    {
        return _timer_fd;
    }

    TimeObject* run_at(int time, const functor& func);

    TimeObject* run_every(int time, const functor& func);

    void cancel(TimeObject* object);

    void stop_timer();

    void on_timer();

private:

    void reset_timer();

    void update_timer(TimeObject* obj);

    int _timer_fd;

    //key = 下次发生定时的时间戳 value = 触发的哪些函数
    std::map<uint64_t, std::list<TimeObject*> > _timer_map;

    uint64_t _cur_timer;

};

}

#endif
