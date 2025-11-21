#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <chrono>
#include <future>
#include <type_traits>

class ThreadPool {
public:
    ThreadPool(int numThreads):stop(false){
        for(int i =0; i<numThreads; i++){
            threads.emplace_back([this] { //用this捕获整个对象传入lambda
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


                    task(); //锁内出入队列 锁外自己玩

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

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        // 把任务包装成 packaged_task<return_type()>
        auto taskPtr = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = taskPtr->get_future();

        {
            std::unique_lock<std::mutex> lock(mtx);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            // 队列里仍然存的是 void()，里面调用 packaged_task
            tasks.emplace([taskPtr]() {
                (*taskPtr)();
            });
        }
        condition.notify_one();

        return res; // 把 future 返回给调用者
    }

private:
    std::vector<std::thread> threads; //线程数组
    std::queue<std::function<void()>> tasks; //任务队列，队列里装的是function
    std::mutex mtx; //声明线程锁
    std::condition_variable condition;   //进行队列通知
    bool stop; //终止命令

};

int main(){
    ThreadPool pool(4);
    for (int i=0; i<5; i++){
        pool.enqueue( [i] {
            std::cout << "task : " << i << " is running" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "task : " << i << " is done" << std::endl;
        }

        );

    }

    auto f2 = pool.enqueue([](int a, int b){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return a+b;
    }, 10,20);



    return 0;
}
