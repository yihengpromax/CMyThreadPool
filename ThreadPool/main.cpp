#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "mythreadpool.h"


// 生产者消费者模型简单实现
std::queue<std::string> g_tasks;
std::condition_variable g_cv;
std::mutex g_mtx;

std::string intToStr(unsigned int iVal)
{
	if (iVal > UINT64_MAX)
	{
		return "The value is so big";
	}
	std::string sRst;
	std::string index[] = {"0", "1", "2", "3", "4", "5","6", "7", "8", "9"};
	sRst = sRst + index[iVal % 10];
	while (iVal > 10)
	{
		iVal = iVal / 10;
		std::string tmp = index[iVal % 10];
		sRst = tmp + sRst;
	}

	return sRst;
}

void producer()
{
	for (int i = 0; i < 200; i++)
	{  
		std::unique_lock<std::mutex> lock(g_mtx);
		g_tasks.push(std::string("task: ") + intToStr(i));
		std::this_thread::sleep_for(std::chrono::microseconds(300)); // 每生产一次任务，让当前线程睡0.5秒
		std::cout << "produce  task: " << i << std::endl;
		g_cv.notify_one();
	}
}

void consumer()
{
	while (1)
	{
		std::unique_lock<std::mutex> lock(g_mtx);
		//if (!lock.try_lock_for(std::chrono::microseconds(2000)))// 锁住临界区资源,两秒还未获取到所有权,则退出
		//{
			//break;
		//}
		g_cv.wait(lock, []() {return !g_tasks.empty();});
		std::string value = g_tasks.front();
		g_tasks.pop();
		std::cout << "consumer " << value << std::endl;
	}
}

int main()
{
	//std::thread p1(producer);
	//std::thread p2(consumer);
	//p1.join();
	//p2.join();

	ThreadPool::CMyThreadPool& newPool = ThreadPool::CMyThreadPool::getInstance(4);
	for (int i = 0; i < 10; i++)
	{
		newPool.addTasks([i] {
			std::cout << "the task: " << i << " is running" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << "the task: " << i << " is done" << std::endl;
			});
	}

	return 0;
}