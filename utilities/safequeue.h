#ifndef OUTER_UTILITIES_SAFEQUEUE_H
#define OUTER_UTILITIES_SAFEQUEUE_H
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>

template <typename T>
class SafeQueue {
   public:
    explicit SafeQueue(const unsigned int& waitTimeoutMsecs)
        : _waitTimeoutMsecs(waitTimeoutMsecs) {
    }
    void setWaitTimeoutMsecs(const unsigned int& secs) {
        _waitTimeoutMsecs = secs;
    }

    void pushBack(const T& item) {
        QMutexLocker locker(&_mutex);
        _queue.push_back(item);
        _condition.notify_one();
    }

    bool popFrontBlockingWithTimeout(T& item) {
        QMutexLocker locker(&_mutex);
        bool hasItem = true;
        if (_queue.isEmpty()) {
            hasItem = _condition.wait(&_mutex, _waitTimeoutMsecs);
        }
        if (hasItem) {
            item = _queue.front();
            _queue.pop_front();
        }
        return hasItem;
    }

    void clear() {
        QMutexLocker locker(&_mutex);
        _queue.clear();
    }

    int size() {
        QMutexLocker locker(&_mutex);
        return _queue.size();
    }

   private:
    QQueue<T> _queue;
    unsigned int _waitTimeoutMsecs;
    QMutex _mutex;
    QWaitCondition _condition;
};

#endif
