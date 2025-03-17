#ifndef MY_THREADPOOL_H
#define MY_THREADPOOL_H
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>


namespace ThreadPool {
/*
	1、初始化：创建线程池对象，初始化任务队列和工作线程。
	2、任务提交：将任务添加到任务队列中。
	3、任务执行：工作线程从任务队列中获取任务并执行。
	4、线程管理：根据需要动态调整线程数量，如增加或减少线程。
	5、关闭线程池：停止所有工作线程，释放资源。
*/
#define MAX_WAITING_TASKS	1000
#define MAX_ACTIVE_THREADS	20
#define BUFSIZE			100
#define PATHSIZE		100


class CMyThreadPool
{
private:
	CMyThreadPool(unsigned int threadsNum);

public:
	~CMyThreadPool();
	CMyThreadPool(const CMyThreadPool& other) = delete;
	CMyThreadPool& operator=(const CMyThreadPool &other) = delete;

	// 线程池应该是单例模式，正常情况下，一个程序最多只能初始化一个线程池实例
	static CMyThreadPool& getInstance(unsigned int threadsNumber);

	template <typename F, typename... Args>
	void addTasks(F&& f, Args&&... args)
	{
		std::function<void()>task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		{
			std::unique_lock<std::mutex> lock(m_mtx);
			m_tasksList.emplace(std::move(task));
		}
		m_cv.notify_one();
	}

	int addThreads(unsigned int addThreadsNum);
	int removeTheads(unsigned int removingThreadsNum);
	bool destroyPool();

private:
	unsigned int m_threadsNum;
	std::mutex m_mtx; // 资源锁
	std::condition_variable m_cv; // 用来模拟生产者消费者的过程

	bool m_shutdown;
	std::queue<std::function<void()>> m_tasksList;
	std::vector<std::thread> m_threadsList;
	//unsigned int m_maxExcuteTasksNum;
	//unsigned int m_maxWaitTasksNum;
	//unsigned int m_activingThreads;
};

} // namespace ThreadPool
#endif // MY_THREADPOOL_H



