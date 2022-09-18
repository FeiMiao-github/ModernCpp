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
        std::suspend_never final_suspend() noexcept
        {
            LOG;
            return {};
        }
        void unhandled_exception() { LOG; }

        unsigned *m_Value;
    };

    std::coroutine_handle<promise_type> m_H;
    operator std::coroutine_handle<>() { return m_H; }
    operator std::coroutine_handle<promise_type>() { return m_H; }
};

template <typename PromiseType>
struct GetPromiseType
{
    PromiseType *m_P;
    bool await_ready() const noexcept
    {
        LOG;
        return false;
    }
    bool await_suspend(std::coroutine_handle<PromiseType> h)
    {
        LOG;
        m_P = &h.promise();
        return false;
    }
    PromiseType *await_resume() const noexcept
    {
        LOG;
        return m_P;
    }
};

ReturnObject counter()
{
    auto pp = co_await GetPromiseType<ReturnObject::promise_type>{};
    unsigned i = 0;
    pp->m_Value = &i;
    for (;; ++i)
    {
        co_await std::suspend_always{};
    }
}

int main()
{
    std::coroutine_handle<ReturnObject::promise_type> h = counter();
    ReturnObject::promise_type *promise = &h.promise();
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "In main function -- " << (*promise->m_Value) << "\n";
        h();
    }
    h.destroy();
    return 0;
}