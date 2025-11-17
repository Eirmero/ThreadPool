#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <chrono>

class ThreadPool {
public:
    ThreadPool(int numThreads):stop(false){
        for(int i =0; i<numThreads; i++){
            threads.emplace_back([this] {
                //  {}当中构造的就是线程体
                while(1){
                    std::unique_lock<std::mutex> lock(mtx);
                    condition.wait(lock,[this] {
                        return !tasks.empty() || stop;
                        });
                    
                    if (stop && tasks.empty()){
                        return;
                        }
                    
                    std::function<void()> task(std::move(tasks.front()));
                    tasks.pop();
                    lock.unlock();
                    task();

                    }

                });
            }
        }

    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        }

        condition.notify_all();
        for(auto& t : threads) {
            t.join();
        }
        
    }

    template<class F, class... Args>
    void enqueue(F &&f, Args&&... args) {
        std::function<void()> task = 
            std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.emplace(std::move(task));
            //指定锁的范围
        }
        condition.notify_one();
    }          //参数操作



private:
    std::vector<std::thread> threads; //线程数组
    std::queue<std::function<void()>> tasks; //任务队列，队列里装的是function
    std::mutex mtx; //声明线程锁
    std::condition_variable condition;   //进行队列通知
    bool stop; //终止命令

};

int main(){
    ThreadPool pool(4);
    for (int i=0; i<10; i++){
        pool.enqueue( [i] {
            std::cout << "task : " << i << " is running" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "task : " << i << " is done" << std::endl;
        }

        );

    }

    return 0;
}