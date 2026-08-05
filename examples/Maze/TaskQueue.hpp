#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

struct Task { std::size_t letterIndex; };

class TaskQueue{
    public:
        void push(Task t);
        bool pop(Task& out);
        void shutdown();
    private:
        std::queue<Task> q_;
        std::mutex m_;
        std::condition_variable cv_;
        bool shutdown_ = false;
};
