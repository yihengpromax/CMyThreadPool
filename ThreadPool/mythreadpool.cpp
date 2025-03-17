#include "mythreadpool.h"

namespace ThreadPool {
	CMyThreadPool::CMyThreadPool(unsigned int threadsNum) : 
		m_threadsNum(threadsNum),
		m_shutdown(false)
	{
		for (int i = 0; i < m_threadsNum; i++)
		{
			m_threadsList.emplace_back([this]() {
			while (1)
			{
				std::unique_lock<std::mutex> lock(m_mtx, std::defer_lock);
				lock.lock();
				m_cv.wait(lock, [this]() {return !m_tasksList.empty() || m_shutdown; });
				if (m_shutdown && m_tasksList.empty())
				{
					return;
				}

				std::function<void()> task(std::move(m_tasksList.front()));
				m_tasksList.pop();
				lock.unlock();
				task(); // 执行
			}
			});
		}
	}

	CMyThreadPool::~CMyThreadPool()
	{
		destroyPool();
	}
	
	CMyThreadPool& CMyThreadPool::getInstance(unsigned int threadsNumber)
	{
		static std::unique_ptr<CMyThreadPool> instance;
		static std::once_flag initFlag;
		if (instance == nullptr)
		{
			std::call_once(initFlag, [threadsNumber]() {
				instance.reset(new CMyThreadPool(threadsNumber));
				});
		}

		return *instance;
	}

	int CMyThreadPool::addThreads(unsigned int addThreadsNum)
	{
		// TODO
		return 0;
	}

	int CMyThreadPool::removeTheads(unsigned int removingThreadsNum)
	{
		// TODO
		return  0;
	}

	bool CMyThreadPool::destroyPool()
	{
		{
			std::unique_lock<std::mutex> lock(m_mtx);
			m_shutdown = true;
		}
		m_cv.notify_all();

		// 等待所有线程执行完毕后，析构掉线程池
		for (auto& t : m_threadsList)
		{
			t.join();
		}

		return true;
	}

}


