#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class ConcurrentQueue {
		std::queue<T> queue_;
		std::mutex mutex_;
		std::condition_variable cond_;
		static constexpr size_t BUFFER_SIZE = 10;

	public:
		ConcurrentQueue() = default;
		ConcurrentQueue(const ConcurrentQueue&) = delete;
		ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

		T pop() {
			std::unique_lock<std::mutex> mlock(mutex_);
			while (queue_.empty()) {
				cond_.wait(mlock);
			}
			auto val = queue_.front();
			queue_.pop();
			mlock.unlock();
			cond_.notify_one();
			return val;
		}

		void push(const T& item) {
			std::unique_lock<std::mutex> mlock(mutex_);
			while (queue_.size() >= BUFFER_SIZE) {
				cond_.wait(mlock);
			}
			queue_.push(item);
			mlock.unlock();
			cond_.notify_one();
		}
};
