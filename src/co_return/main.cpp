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

struct ReturnObject
{
    struct promise_type
    {
        promise_type() { LOG; }

        ~promise_type() { LOG; }

        ReturnObject get_return_object()
        {
            LOG;
            return {
                .m_H
                = std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend()
        {
            LOG;
            return {};
        }
        std::suspend_always final_suspend() noexcept
        {
            LOG;
            return {};
        }
        void unhandled_exception() { LOG; }

        std::suspend_always yield_value(unsigned value)
        {
            m_Value = value;
            LOG;
            return {};
        }

        void return_void() {}

        unsigned m_Value;
    };

    std::coroutine_handle<promise_type> m_H;
    // operator std::coroutine_handle<>() { return m_H; }
    // operator std::coroutine_handle<promise_type>() { return m_H; }
};

ReturnObject counter()
{
    for (unsigned i = 0; i < 3; ++i)
    {
        // co_await promise->yield_value(i);
        co_yield i;
    }
}

int main()
{
    using promise_type = ReturnObject::promise_type;
    using coroutine_handle = std::coroutine_handle<promise_type>;

    coroutine_handle h = counter().m_H;
    promise_type &promise = h.promise();
    while (!h.done())
    {
        std::cout << "In main function -- " << (promise.m_Value) << "\n";
        h();
    }
    h.destroy();
    return 0;
}
