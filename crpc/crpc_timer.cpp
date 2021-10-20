#include "crpc_timer.h"

namespace crpc
{

struct TimeObject {
    uint64_t tick_time;
    uint64_t next_time;
    functor func;
    bool repeated;
};

TimeObject* CRpcTimer::run_at(int time, const functor& func)
{
    _cur_timer = get_timestamps_ms();

    TimeObject* obj = new TimeObject;
    obj->func = func;
    obj->next_time = _cur_timer + time;
    obj->repeated = false;
    obj->tick_time = time;

    //update next tick
    update_timer(obj);
}

TimeObject* CRpcTimer::run_every(int time, const functor& func)
{
    _cur_timer = get_timestamps_ms();

    TimeObject* obj = new TimeObject;
    obj->func = func;
    obj->next_time = _cur_timer + time;
    obj->repeated = true;
    obj->tick_time = time;

    //update next tick
    update_timer(obj);
}

void CRpcTimer::on_timer()
{
    _cur_timer = get_timestamps_ms();
    std::list<TimeObject*> obj_list;
    auto itr = _timer_map.begin();

    if (itr == _timer_map.end())
        return;

    std::list<TimeObject*> &ref = itr->second;
    for (TimeObject* obj : ref)
    {
        obj->func();
        if (obj->repeated)
        {
            obj->next_time = _cur_timer + obj->tick_time;
            obj_list.push_back(obj);
        }
        else
        {
            delete obj;
        }
    }

    _timer_map.erase(itr);
    if (!obj_list.empty())
    {
        auto &ref_list = _timer_map[_cur_timer + obj_list.front()->tick_time];
        ref_list.splice(ref_list.begin(), obj_list);
    }

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

    //秒 纳秒
    uint64_t next_time = _timer_map.begin()->first;

    //说明已经超时了，需要马上执行了
    if (next_time <= _cur_timer)
    {
        crpc_log("not a good thing!!!\n");
        on_timer();
        return;
    }

    new_value.it_value.tv_sec = (next_time - _cur_timer) / 1000;
    new_value.it_value.tv_nsec = ((next_time - _cur_timer) % 1000) * 1000000;
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


