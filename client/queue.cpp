#include "client.h"

/*class c_queue {
	std::queue <c_trans_t> Q;
	unsigned int q_max_size;
	std::mutex mtx;
	std::condition_variable pcv;	
	std::condition_variable ccv;	
public:
	int insert_trans(c_trans_t *item);
	c_trans_t *remove_trans();
};*/

/*
 * sets the maximum size of the queue.
 */
c_queue::c_queue(unsigned int max)
{
	this->q_max_size = max;
}

/*
 * Insert a transaction in a queue.
 * If a queue is full, wait on a confition variable.
 */
int c_queue::insert_trans(c_trans_t *t)
{
	std::unique_lock<std::mutex> lck(this->mtx);
	/* Q is full */
	if(this->Q.size() == this->q_max_size)
		this->pcv.wait(lck);
	this->Q.push(t);
	lck.unlock();
	this->ccv.notify_one();
	return 0;
}

/*
 * Try to pop a transaction from a queue.
 * If queue is empty, it waits.
 */
c_trans_t *c_queue::remove_trans()
{
	c_trans_t *t;
	std::unique_lock<std::mutex> lck(this->mtx);

	if(Q.empty())
		this->ccv.wait(lck);

	t = this->Q.front();
	this->Q.pop();
	lck.unlock();

	this->pcv.notify_one();
	return t;
}
