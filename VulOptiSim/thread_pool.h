#pragma once

//You can use this threadpool class in your code to reduce the overhead of spawning threads.
//  Unfortunately it's missing a notification mechanism (e.g. waking up a worker when a task is pushed). 
//  To use this thread pool you need to fix this issue before the threadpool works.
//  Discuss with your fellow students how this can be done. :)

class ThreadPool; //Forward declare

class Worker
{
public:
    //Instantiate the worker class by passing and storing the threadpool as a reference
    explicit Worker(ThreadPool& s) : pool(s) {}

    void operator()();

private:
    ThreadPool& pool;
};

class ThreadPool
{
public:
    explicit ThreadPool(size_t numThreads)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers.push_back(std::thread(Worker(*this)));
        }
    }

    ~ThreadPool()
    {
        stop = true; // stop all threads

        for (auto& thread : workers)
            thread.join();
    }


    template <class T>
    [[nodiscard]] auto enqueue(T task) -> std::future<decltype(task())>
    {
        //Wrap the function in a packaged_task so we can return a future object
        auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));

        //Scope to restrict critical section
        {
            //lock our queue and add the given task to it
            std::unique_lock<std::mutex> lock(queue_mutex);

            tasks.push_back([=] {(*wrapper)(); });
        }

        return wrapper->get_future();
    }

private:
    friend class Worker; //Gives access to the private variables of this class

    std::vector<std::thread> workers;
    std::deque<std::function<void()>> tasks;


    std::mutex queue_mutex; //Lock for our queue
    bool stop = false;
};

inline void Worker::operator()()
{
    std::function<void()> task;
    while (true)
    {
        //Scope to restrict critical section
        //This is important because we don't want to hold the lock while executing the task,
        //because that would make it so only one task can be run simultaneously (aka sequantial)
        {
            std::unique_lock<std::mutex> locker(pool.queue_mutex);


            if (pool.stop) break;

            task = pool.tasks.front();
            pool.tasks.pop_front();
        }

        task();
    }
}