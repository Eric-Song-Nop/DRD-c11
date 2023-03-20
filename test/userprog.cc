#include <iostream>
#include <thread>
#include <atomic>

std::atomic<int> count{0};

void incrementCount() {
    for (int i = 0; i < 5; i++) {
        count++;
    }
}

int main() {
    std::thread t1(incrementCount);
    std::thread t2(incrementCount);
    
    t1.join();
    t2.join();
    
    std::cout << "Count: " << count << std::endl;
    
    return 0;
}