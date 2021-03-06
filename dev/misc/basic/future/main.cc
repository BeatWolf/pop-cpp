#include <iostream>
#include <future>
#include <thread>
#include <unistd.h>

int main() {
    // future from a packaged_task
    std::packaged_task<int()> task([]() {
        sleep(2);
        return 7;
    });                                       // wrap the function
    std::future<int> f1 = task.get_future();  // get a future
    std::thread(std::move(task)).detach();    // launch on a thread

    // future from an async()
    std::future<int> f2 = std::async(std::launch::async, []() {
        sleep(2);
        return 8;
    });

    // future from a promise
    std::promise<int> p;
    std::future<int> f3 = p.get_future();
    std::thread([](std::promise<int>& p) { p.set_value(9); }, std::ref(p)).detach();

    std::cout << "Waiting...";
    f1.wait();
    f2.wait();
    f3.wait();
    std::cout << "Done!\nResults are: " << f1.get() << ' ' << f2.get() << ' ' << f3.get() << '\n';
}