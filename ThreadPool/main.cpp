#include <future>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <type_traits>



/* Non-thread-safe, affects readability. */
constexpr const bool PRINT_INFO = false;

class ThreadPool{
public:
    /* Construct function */
    ThreadPool(size_t nums_worker = std::thread::hardware_concurrency()): 
        should_stop_(false)
    {
        std::cout << "Thread number:" << nums_worker << std::endl;
        workers_.reserve(nums_worker);
        for (size_t i = 0; i < nums_worker; ++i){
            workers_.emplace_back([this, i](){
                while (true){
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->task_mutex_);
                        this->cv_.wait(lock, [this](){
                            return this->should_stop_ || !this->tasks_.empty();
                        });
                        if (this->should_stop_ && this->tasks_.empty()){
                            return;
                        }
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop_front();
                        if constexpr (PRINT_INFO) std::cout << "task will be run at thrad:" << i << std::endl;
                    }
                    task();
                    if constexpr (PRINT_INFO) std::cout << "task finished at thread:" << i << std::endl;
                }
            });
        }
    }

    /* Destruct function*/
    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(this->task_mutex_);
            should_stop_ = true;
        }
        cv_.notify_all();
        for (auto& worker : workers_){
            worker.join();
        }
    }

    /* Push task to the front of the task queue */
    template<typename Func, typename... Args>
    auto push_front(Func&& func, Args&&... args){
        return push<Func, true, Args...> (std::forward<Func>(func), std::forward<Args>(args)...);
    }

    /* Push task to the end of the task queue */
    template<typename Func, typename... Args>
    auto push_back(Func&& func, Args&&... args){
        return push<Func, false, Args...> (std::forward<Func>(func), std::forward<Args>(args)...); 
    }


private:
    bool                                should_stop_;
    std::vector<std::thread>            workers_;
    std::deque<std::function<void()>>   tasks_;
    std::mutex                          task_mutex_;
    std::condition_variable             cv_;
    

    /* Push function */
    template<typename Func, bool is_front,  typename... Args>
    auto push(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type>{
        using return_type = typename std:: invoke_result<Func, Args...>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
        [f = std::forward<Func>(func), tuple_args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
            return std::apply(f, std::move(tuple_args));
        });
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(task_mutex_);
            if (should_stop_){
                throw std::runtime_error("push task on stopped ThreadPool");
            }
            if constexpr (is_front){
                tasks_.emplace_front([task](){ (*task)(); });
            }
            else{
                tasks_.emplace_back([task](){ (*task)(); });
            }
        }
        cv_.notify_one();
        return res;
    };


};


using namespace std;
int main(){

    ThreadPool pool{};
    /* Task1 : Calculate sum */
    auto task1 = [](int x){
        int res = 0;
        for (int i = 1; i <= x; ++i){
            res += i;
        }
        return res;
    };
    std::future<int> task1_res = pool.push_back(task1, 10000);
    std::cout << "task1 result:" <<task1_res.get() << std::endl;


    /* Task2 : Count odd number*/
    auto task2 = [](int x, int y){
        if (x > y){
            return -1;
        }
        int res = 0;
        while (x <= y){
            if (x & 1){
                res ++;
            }
            x ++;
        }
        return res;
    };
    std::future<int> task2_res = pool.push_back(task2, 10000, 12345);
    std::cout << "task2 result:" <<task2_res.get() << std::endl;


    /* Task 3 : Calculate factorial*/
    auto task3 = [&](auto &&self, long long x) -> long long {
        if (x == 1){
            return x;
        }
        return x * self(self, x - 1);
    };
    std::future<long long> task3_res = pool.push_back(task3, task3, 15);
    std::cout << "task3 result:" << task3_res.get() << std::endl;


    /* Task 4 : Output args */
    auto create_task4 = []() {
        return [](auto&& self, auto&& value, auto&&... args)->bool {
            std::cout << value << ' ';
            if constexpr (sizeof...(args) > 0) {
                return self(self, std::forward<decltype(args)>(args)...);
            }
            else{
                std::cout << ' ';
                return true;
            }
        };
    };
    auto task4 = create_task4();
    std::future<bool> task4_res = pool.push_back(task4, task4, "The", "world", "will", "be", "more", "beautiful", 0, "w", 0);
    std::cout << "task4 result:" << (task4_res.get() ? "Output over" : "Output unfinished") << std::endl;


    /* Task 5 : Check variable type */
    auto task5 = [](auto&& self, auto&& pre, auto&& cur, auto&&... args) -> bool {
        if constexpr (!std::is_same_v<std::decay_t<decltype(pre)>, std::decay_t<decltype(cur)>>) {
            return false;
        }
        if constexpr (sizeof...(args) <= 1) {
            return std::is_same_v<std::decay_t<decltype(cur)>, std::decay_t<decltype(args)>...>;
        }
        else{
            return self(self, std::forward<decltype(cur)>(cur), std::forward<decltype(args)>(args)...);
        }
    };
    std::future<bool> task5_res = pool.push_front(task5, task5, "The", "world", "will", "be", "more", "beautiful", 0, "w", 0);
    std::cout << "task5 result:" << (!task5_res.get() ? "Variables are different type" : "Variables are Same type") << std::endl;


    return 0;
}