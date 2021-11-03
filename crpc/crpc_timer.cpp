#include "crpc_timer.h"

namespace crpc
{

struct TimeObject {
    uint64_t tick_time;
    uint64_t next_time;
    functor func;
    bool repeated;
};

uint64_t CRpcTimer::get_tick_timer()
{
    if (_timer_map.empty())
        return 0;

    assert(!_timer_map.begin()->second.empty());
    TimeObject* obj = _timer_map.begin()->second.front();

    struct itimerspec curr_value;
    timerfd_gettime(_timer_fd, &curr_value);

    //assert(obj->tick_time >= curr_value.it_value);
    return obj->tick_time - (curr_value.it_value.tv_sec * 1000 + curr_value.it_value.tv_nsec / 1000);
}

TimeObject* CRpcTimer::run_at(int time, const functor& func)
{
    if (time <= 0)
        time = 1;

    _tick_timer += get_tick_timer();
    TimeObject* obj = new TimeObject;
    obj->func = func;
    obj->next_time = _tick_timer + time;
    obj->repeated = false;
    obj->tick_time = time;

    //update next tick
    update_timer(obj);
    return obj;
}

TimeObject* CRpcTimer::run_every(int time, const functor& func)
{
    if (time <= 0)
        time = 1;
 
    _tick_timer += get_tick_timer();
    TimeObject* obj = new TimeObject;
    obj->func = func;
    obj->next_time = _tick_timer + time;
    obj->repeated = true;
    obj->tick_time = time;

    //update next tick
    update_timer(obj);
    return obj;
}

void CRpcTimer::on_timer()
{
    auto itr = _timer_map.begin();
    if (itr == _timer_map.end())
        return;

    std::list<TimeObject*> &ref = itr->second;
    assert(!ref.empty());

    //更新时间戳
    _tick_timer = itr->first;

    for (TimeObject* obj : ref)
    {
        obj->func();
        if (obj->repeated)
        {
            obj->next_time = _tick_timer + obj->tick_time;
            _timer_map[obj->next_time].push_back(obj);
        }
        else
        {
            delete obj;
        }
    }

    _timer_map.erase(itr);
    reset_timer();
}

void CRpcTimer::reset_timer()
{
    itimerspec new_value;
    memset(&new_value, 0, sizeof(new_value));

    if (_timer_map.empty())
    {
        //stop timer
        timerfd_settime(_timer_fd, 0, &new_value, NULL);
        return;
    }

    assert(!_timer_map.begin()->second.empty());
    
    uint64_t diff = 1;
    if (_timer_map.begin()->first > _tick_timer)
        diff = _timer_map.begin()->first - _tick_timer;

    new_value.it_value.tv_sec = diff / 1000;
    new_value.it_value.tv_nsec = (diff % 1000) * 1000000;
    timerfd_settime(_timer_fd, 0, &new_value, NULL);
}

void CRpcTimer::update_timer(TimeObject* object)
{
    _timer_map[object->next_time].push_back(object);
    reset_timer();
}

void CRpcTimer::cancel(TimeObject* object)
{
    uint64_t next_time = object->next_time;
    auto itr = _timer_map.find(object->next_time);
    if (itr == _timer_map.end())
        return;

    auto obj_itr = find(itr->second.begin(), itr->second.end(), object);
    if (obj_itr == itr->second.end())
        return;

    itr->second.erase(obj_itr);
    delete object;

    if (itr->second.empty())
    {
        _timer_map.erase(next_time);
    }

    reset_timer();
}

}


