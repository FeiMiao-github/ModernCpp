#include <chrono>
#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>
#include <typeinfo>

using namespace std::chrono;
using namespace std::chrono_literals;

#define LOG                                                                    \
    std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << " run !" << std::endl

struct ReturnObject {
    struct promise_type {
        ReturnObject get_return_object() {
            LOG;
            return {};
        }
        std::suspend_never initial_suspend() {
            LOG;
            return {};
        }
        std::suspend_never final_suspend() noexcept {
            LOG;
            return {};
        }
        void unhandled_exception() { LOG; }
    };
};

struct Awaiter {
    std::coroutine_handle<> *hp_;
    bool await_ready() const noexcept {
        LOG;
        return false;
    }
    void await_suspend(std::coroutine_handle<> h) {
        *hp_ = h;
        std::this_thread::sleep_for(1s);
        LOG;
    }
    void await_resume() const noexcept { LOG; }
};

ReturnObject counter(std::coroutine_handle<> *continuation_out) {
    Awaiter a{continuation_out};
    for (unsigned i = 0;; ++i) {
        co_await a;
        std::this_thread::sleep_for(1s);
        std::cout << "counter: " << i << std::endl;
    }
}

int main() {
    std::coroutine_handle<> h;
    std::cout << typeid(h).name() << '\n';
    counter(&h);
    for (int i = 0; i < 3; ++i) {
        std::cout << "In main function\n";
        h();
    }
    h.destroy();
}