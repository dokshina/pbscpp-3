#include <iostream>
#include <thread>
#include <queue>
#include <condition_variable>

int main() {
    size_t count = 0;
    bool done = false;
    std::queue<int> items;

    bool notified = false;
    std::mutex m; // items and count mutex
    std::condition_variable check;

    std::thread producer([&]() {
        for (int i = 0; i < 5; ++i) {
            std::unique_lock<std::mutex> lock(m);

            std::cout << "I am push!" << std::endl;
            items.push(i);
            count++;

            notified = true;
            check.notify_one();
        }
        done = true;
    });

    std::thread consumer([&]() {
        while (!done) {
            std::unique_lock<std::mutex> lock(m);

            while (!notified) check.wait(lock);

            while (!items.empty()) {
                std::cout << "I am pop!" << std::endl;

                items.pop();
                count--;
            }

            notified = false;
        }
    });

    producer.join();
    consumer.join();
    std::cout << count << std::endl;
}
