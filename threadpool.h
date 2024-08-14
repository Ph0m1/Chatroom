#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

template <typename T> class SafeQueue {
private:
    std::queue<T> m_queue; // 队列

    std::mutex m_mutex; // 互斥锁

public:
    SafeQueue() {}
    SafeQueue(SafeQueue &&other) {}
    ~SafeQueue() {}

    bool empty() { // 判断队列是否为空
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    int size() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    void enqueue(T &t) {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_queue.emplace(t);
    }

    bool dequeue(T &t) {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty()) {
            return false;
        }
        t = std::move(
            m_queue.front()); // 取出队首元素，返回队首元素值，并进行右值引用
        m_queue.pop(); // 弹出队首元素
        return true;
    }
};

class ThreadPool {
private:
    class ThreadWorker {
    private:
        int m_id; // 工作id

        ThreadPool *m_pool; // 所属线程池

    public:
        ThreadWorker(ThreadPool *pool, const int id) : m_pool(pool), m_id(id) {}
        void operator()() {
            std::function<void()> func; // 定义函数基类
            bool dequeued;              // 是否正在取出队列中的元素

            while (!m_pool->m_shutdown) {
                {
                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
                    if (m_pool->m_queue.empty()) {
                        m_pool->m_conditional_lock.wait(lock); // 等待条件变量通知开启线程。
                    }
                    dequeued = m_pool->m_queue.dequeue(func);
                }
                // 成功取出再执行工作函数
                if (dequeued) {
                    func();
                }
            }
        }
    };
    bool m_shutdown;                            // 线程池是否关闭
    SafeQueue<std::function<void()>> m_queue;   // 任务队列
    std::vector<std::thread> m_threads;         // 工作线程队列
    std::mutex m_conditional_mutex;             // 线程休眠互斥锁
    std::condition_variable m_conditional_lock; // 线程环境锁

public:
    ThreadPool(const int n_threads = 4)
        : m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false) {}

    ThreadPool(const ThreadPool &) = delete;

    ThreadPool(ThreadPool &&) = delete;

    ThreadPool &operator=(const ThreadPool &) = delete;

    ThreadPool &operator=(ThreadPool &&) = delete;
    // 初始化线程池
    void init() {
        for (int i = 0; i < m_threads.size(); i++) {
            m_threads.at(i) = std::thread(ThreadWorker(this, i)); // 分配工作线程
        }
    }

    void shutdown() {
        m_shutdown = true;
        m_conditional_lock.notify_all(); // 唤醒所有线程
        for (int i = 0; i < m_threads.size(); i++) {
            if (m_threads.at(i).joinable()) { // 判断线程是否在等待
                m_threads.at(i).join();
            }
        }
    }

    // 提交函数模板
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args)->std::future<decltype(f(args...))>{
        // 绑定函数及其参数
        std::function<decltype(f(args...))()>func = std::bind(std::forward<F>(f), std::forward<Args>(args)...); // 连接函数和参数定义，特殊函数类型，避免左右值错误
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        std::function<void()> warpper_func = [task_ptr](){
            (*task_ptr)();
        };
        m_queue.enqueue(warpper_func); // 封装函数并压入队列
        m_conditional_lock.notify_one(); // 通知线程池有任务
        return task_ptr->get_future(); // 返回任务指针

    }
    ~ThreadPool() {
        // m_shutdown = true;
        shutdown();
    }
};
#endif // THREADPOOL_H
