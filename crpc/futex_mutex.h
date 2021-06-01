#ifndef CRPC_FUTEXT_MUTEX_H
#define CRPC_FUTEXT_MUTEX_H

#include <linux/futex.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sched.h>
#include <sys/resource.h>

//Ŀǰֻ֧��һ���߳�������һ���߳�����
//TODO Ŀǰ��ģ�ʹ��ռ����8%��ʹ���ʣ���Ҫ�Ż�һ��
/*
-    7.81%     0.01%  rpc_server  [kernel.kallsyms]      [k] sys_futex
   - 7.79% sys_futex            
      - 7.79% do_futex          
         - 7.66% futex_wake     
            - 7.65% wake_up_q   
               - try_to_wake_up 
                    7.62% _raw_spin_unlock_irqrestore
*/
class FutextMutex
{
public:
    FutextMutex():_producer(0), _consumer(0)
    {}

    int signal()
    {
        if (get_expected(_producer) == get_expected(_consumer))
        {
            _producer.fetch_add(1, std::memory_order_release);
            syscall(SYS_futex, &_producer, (FUTEX_WAKE | FUTEX_PRIVATE_FLAG),
                   1, NULL, NULL, 0);
        }
        else
        {
            _producer.fetch_add(1, std::memory_order_release);
        }
        return 0;
    }

    void wait()
    {
        int produce_val = get_expected(_producer);
        int consumer_val = get_expected(_consumer);
        if (produce_val == consumer_val)
        {
            syscall(SYS_futex, &_producer, (FUTEX_WAIT | FUTEX_PRIVATE_FLAG),
                    produce_val, NULL, NULL, 0);
        }
        else
        {
            //�˴�һ���ɹ�
            while(!_consumer.compare_exchange_strong(consumer_val, produce_val, 
            std::memory_order_relaxed));
        }
    }

private:
    int get_expected(std::atomic<int>& ref)
    {
        return ref.load(std::memory_order_acquire);
    }

private:
    std::atomic<int> _producer;
    std::atomic<int> _consumer;
};

#endif
