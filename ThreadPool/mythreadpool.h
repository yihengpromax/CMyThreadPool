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
	1����ʼ���������̳߳ض��󣬳�ʼ��������к͹����̡߳�
	2�������ύ����������ӵ���������С�
	3������ִ�У������̴߳���������л�ȡ����ִ�С�
	4���̹߳���������Ҫ��̬�����߳������������ӻ�����̡߳�
	5���ر��̳߳أ�ֹͣ���й����̣߳��ͷ���Դ��
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

	// �̳߳�Ӧ���ǵ���ģʽ����������£�һ���������ֻ�ܳ�ʼ��һ���̳߳�ʵ��
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
	std::mutex m_mtx; // ��Դ��
	std::condition_variable m_cv; // ����ģ�������������ߵĹ���

	bool m_shutdown;
	std::queue<std::function<void()>> m_tasksList;
	std::vector<std::thread> m_threadsList;
	//unsigned int m_maxExcuteTasksNum;
	//unsigned int m_maxWaitTasksNum;
	//unsigned int m_activingThreads;
};

} // namespace ThreadPool
#endif // MY_THREADPOOL_H



