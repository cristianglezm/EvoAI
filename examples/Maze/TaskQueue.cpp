#include "TaskQueue.hpp"

void TaskQueue::push(Task t){
    {
		std::lock_guard<std::mutex> lk(m_);
		q_.push(std::move(t));
	}
    cv_.notify_one();
}
bool TaskQueue::pop(Task& out){
    std::unique_lock<std::mutex> lk(m_);
    cv_.wait(lk, 
		[&]{
			return shutdown_ || !q_.empty(); 
		}
	);
    if(q_.empty()){
		return false;
	}
    out = std::move(q_.front()); 
	q_.pop();
    return true;
}
void TaskQueue::shutdown(){
    { 
		std::lock_guard<std::mutex> lk(m_);
		shutdown_ = true;
	}
    cv_.notify_all();
}
