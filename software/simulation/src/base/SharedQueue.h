#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class SharedQueue {
    std::deque<T> queue;
    std::mutex mutex;
    std::condition_variable cond;

public:
    SharedQueue();

    T & front();
    void pop_front();

    void push_back(const T &item);
    void push_back(T &&item);

    int size();
    bool empty();
};

template <typename T>
SharedQueue<T>::SharedQueue() {

}

template <typename T>
T & SharedQueue<T>::front() {
    std::unique_lock<std::mutex> lock(mutex);

    while(queue.empty()) {
        cond.wait(lock);
    }

    return queue.front();
}

template <typename T>
void SharedQueue<T>::pop_front() {
    std::unique_lock<std::mutex> lock(mutex);

    while(queue.empty()) {
        cond.wait(lock);
    }

    queue.pop_front();
}

template <typename T>
void SharedQueue<T>::push_back(const T &item) {
    std::unique_lock<std::mutex> lock(mutex);

    queue.push_back(item);
    lock.unlock();
    cond.notify_one();
}

template <typename T>
void SharedQueue<T>::push_back(T &&item) {
    std::unique_lock<std::mutex> lock(mutex);

    queue.push_back(std::move(item));
    lock.unlock();
    cond.notify_one();
}

template <typename T>
int SharedQueue<T>::size() {
    std::unique_lock<std::mutex> lock(mutex);

    const int size = queue.size();

    lock.unlock();

    return size;
}
