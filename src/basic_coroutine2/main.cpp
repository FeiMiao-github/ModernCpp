#include <coroutine>
#include <iostream>

#define LOG std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << " run !" << std::endl

struct ReturnObj
{
    struct promise_type
    {
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

        void unhandled_exception()
        {
            LOG;
        }

        ReturnObj get_return_object()
        {
            LOG;
            return {
                .m_h = std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }
    };
    
    std::coroutine_handle<promise_type> m_h;
    // operator std::coroutine_handle<promise_type>() { return m_h; }
    operator std::coroutine_handle<>() { return m_h; }
};

ReturnObj counter()
{
    for (unsigned i = 0;; ++i) {
        co_await std::suspend_always{};
        std::cout << "==== i " << i << " =====\n";
    }
}

int main()
{
    std::coroutine_handle<> h = counter();

    for (unsigned i = 0; i < 3; i++)
        h();

    h.destroy();
    return 0;
}