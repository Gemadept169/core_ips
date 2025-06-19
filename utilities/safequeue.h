#ifndef OUTER_UTILITIES_SAFEQUEUE_H
#define OUTER_UTILITIES_SAFEQUEUE_H

#include <mutex>
#include <queue>

template <typename T>
class SafeQueue {
   public:
    explicit SafeQueue() {
    }

    void pushBack(const T& item) {
        std::lock_guard<std::mutex> locker(_mutex);
        _queue.push(item);
    }

    bool popFront(T& item) {
        std::lock_guard<std::mutex> locker(_mutex);
        bool hasItem = false;
        if (!_queue.empty()) {
            item = _queue.front();
            _queue.pop();
            hasItem = true;
        }
        return hasItem;
    }

    void clear() {
        std::lock_guard<std::mutex> locker(_mutex);
        while (!_queue.empty()) {
            _queue.pop();
        }
    }

    int size() {
        std::lock_guard<std::mutex> locker(_mutex);
        return _queue.size();
    }

   private:
    std::queue<T> _queue;
    std::mutex _mutex;
};

#endif
