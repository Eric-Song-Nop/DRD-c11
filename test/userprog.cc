#include <iostream>
#include <thread>
#include <atomic>

// int b = 0;
std::atomic<int> count{0};

void incrementCount()
{
    for (int i = 0; i < 5; i++)
    {
        count += i;
        // b++;
    }
    std::atomic<int> c{0};
    for (int i = 0; i < 5; i++)
        c += i;
    std::cout << "c: " << c << std::endl;
}

int main()
{
    std::thread t1(incrementCount);
    std::thread t2(incrementCount);

    t1.join();
    t2.join();

    std::cout << "Count: " << count << std::endl;
    // std::cout << "a: " << b << std::endl;

    return 0;
}