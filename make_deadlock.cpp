#include <iostream>
#include <mutex>
#include <thread>
#include <string>
#include <optional>

using std::lock_guard;
using std::thread;
using std::mutex;
using std::cout;
using std::endl;

using namespace std::chrono_literals;

// Common variables
std::optional<int> WorkTime; // Mode. If not set - produce deadlock, if set - idle work for *WorkTime seconds
mutex m1, m2;         // Mutexes
thread t2;            // Second thread

// Print current thread id and return cout
std::ostream& Out()
{
    const std::thread::id id = std::this_thread::get_id();
    return cout << "Thread " << id << ": ";
}

// Just wait n times for t duration and print message
void DoNothing(int n, std::chrono::milliseconds t)
{
    for (int i=0; i<n; ++i) {
        std::this_thread::sleep_for(t);
        Out() << "Doing nothing for " << t.count() << "ms, #" << i << "." << endl;
    }
}

// Procedure for second thread
void P2();

// Procedure for first thread
void P1()
{
    Out() << "This is a first thread." << endl;
    Out() << "Locking mutex m1" << endl;
    lock_guard g1(m1);
    Out() << "Owns mutex m1" << endl;

    if (!WorkTime) {  // If WorkTime is not set, produce deadlock 
        t2 = thread{P2};     // Start second thread 
        DoNothing(1, 100ms); // And wait a little to let second thread will own mutex m2. Not a very reliable solution.
    }

    Out() << "Locking mutex m2" << endl;
    lock_guard g2(m2);
    Out() << "Owns mutex m2" << endl;
}

void P2()
{
    Out() << "This is a second thread." << endl;
    Out() << "Locking mutex m2" << endl;
    lock_guard g2(m2);
    Out() << "Owns mutex m2" << endl;

    Out() << "Locking mutex m1" << endl;
    lock_guard g1(m1);
    Out() << "Owns mutex m1" << endl;
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        try {
            WorkTime = std::stoi(argv[1]);
        } catch (...) {
        }
    }

    Out() << "This is a main thread." << endl;
    P1();

    if (WorkTime)
        DoNothing(*WorkTime, 1s);

    return EXIT_SUCCESS;
}
