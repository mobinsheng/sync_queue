#include <iostream>
#include <thread>
#include <chrono>
#include "sync_queue.h"

using namespace std;

SyncQueue<int> Q;

void produce(){
    for(int i = 0; i < 1024; ++i){
        Q.push_back(i);
    }
    Q.stop();
}

void consume(){
    int val;
    while (Q.pop_front(val)) {
        printf("val = %d\n",val);
        std::this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main()
{
    thread pth(produce);
    pth.detach();

    thread cth1(consume);
    thread cth2(consume);
    thread cth3(consume);
    thread cth4(consume);
    thread cth5(consume);
    thread cth6(consume);
    thread cth7(consume);
    thread cth8(consume);

    cth1.join();
    cth2.join();
    cth3.join();
    cth4.join();
    cth5.join();
    cth6.join();
    cth7.join();
    cth8.join();

    return 0;
}
