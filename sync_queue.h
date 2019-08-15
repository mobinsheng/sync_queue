#ifndef SYNC_QUEUE_H
#define SYNC_QUEUE_H

/*
 * 同步队列，实现了生产者消费者
 */
#include <deque>
#include <pthread.h>


template<typename T>
class SyncQueue{
public:
    SyncQueue(){
        running_ = true;
        pthread_mutex_init(&mutex_,NULL);
        pthread_cond_init(&cond_,NULL);
    }

    ~SyncQueue(){
        stop();
        pthread_cond_destroy(&cond_);
        pthread_mutex_destroy(&mutex_);
    }

    // 入队
    void push_back(T info){
        SafeLock guard(mutex_);
        if (!running_) {
            return;
        }

        bool empty = data_list_.empty();
        data_list_.push_back(info);
        if (empty) {
            Signal();
        }
    }

    // 返回第一个元素，不等待
    bool head(T& val) {
        SafeLock guard(mutex_);
        if ( data_list_.empty()) {
            return false;
        }

        val = data_list_[0];
        return true;
    }

    // 返回最后一个元素，不等待
    bool tail(T& val) {
        SafeLock guard(mutex_);
        if (data_list_.empty()) {
            return false;
        }

        val = data_list_[data_list_.size() -1];

        return true;
    }

    // 获取某个元素，不等待
    bool get(size_t index, T& val) {
        SafeLock guard(mutex_);
        if (data_list_.empty()) {
            return false;
        }

        if (index >= data_list_.size()) {
            return false;
        }

        val = data_list_[index];

        return true;
    }

    // 出队，等待
    bool pop_front(T& val){
        SafeLock guard(mutex_);
        if (!running_ && data_list_.empty()) {
            return false;
        }

        bool ret = false;

        // 已经不运行了
        if (!running_) {
            if (data_list_.empty() == false) {
                val = data_list_[0];
                data_list_.pop_front();
                ret = true;
            }
            else {
                ret = false;
            }

            return ret;
        }

        // 还在运行
        Wait();

        // 到了这一步将有两种情况：
        // 1、队列中有数据
        // 2、队列退出!
        if (running_) {
            if (!data_list_.empty()) {
                val = data_list_[0];
                data_list_.pop_front();
                ret = true;
            }
            else {
                // 一般来说这一步是不存在的
                ret = false;
            }

        }
        else {
            ret = false;
        }

        return ret;
    }

    size_t size(){
        SafeLock guard(mutex_);
        size_t size = data_list_.size();
        return size;
    }

    bool empty(){
        SafeLock guard(mutex_);
        bool empty = data_list_.empty();
        return empty;
    }

    bool is_running() {
        SafeLock guard(mutex_);
        return running_;
    }

    /*void start() {
        SafeLock guard(mutex_);
        if(running_){
            return;
        }

        //data_list_.clear();
        running_ = true;
    }*/

    // 由数据发送方调用
    void stop() {
        SafeLock guard(mutex_);
        running_ = false;
        Broadcast();
    }

private:
    void Wait(){
        while(data_list_.empty() && running_){
            pthread_cond_wait(&cond_,&mutex_);
        }
    }

    void Signal(){
        pthread_cond_signal(&cond_);
    }

    void Broadcast() {
        pthread_cond_broadcast(&cond_);
    }

    class SafeLock {
    public:
        SafeLock(pthread_mutex_t& l):mutex(l) {
            pthread_mutex_lock(&mutex);
        }
        ~SafeLock() {
            pthread_mutex_unlock(&mutex);
        }
    private:
        pthread_mutex_t& mutex;
    };
private:
    SyncQueue & operator=(const SyncQueue&) {}

    // 队列锁和队列
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    std::deque<T> data_list_;

    bool running_;
};


#endif // SYNC_QUEUE_H
