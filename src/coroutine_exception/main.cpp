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

struct Generator
{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        Generator get_return_object()
        {
            LOG;
            return Generator(handle_type::from_promise(*this));
        }
        std::suspend_always initial_suspend()
        {
            LOG;
            return {};
        }
        std::suspend_always final_suspend() noexcept
        {
            LOG;
            return {};
        }
        void unhandled_exception()
        {
            LOG;
            m_Exception = std::current_exception();
        }

        template <std::convertible_to<unsigned> From>
        std::suspend_always yield_value(From &&value)
        {
            m_Value = std::forward<From>(value);
            LOG;
            return {};
        }

        unsigned m_Value;
        std::exception_ptr m_Exception;
    };

    handle_type m_H;
    bool m_Full;

    operator std::coroutine_handle<>() { return m_H; }
    operator handle_type() { return m_H; }

    Generator(handle_type h) : m_H(h), m_Full(false) { LOG; }

    ~Generator()
    {
        m_H.destroy();
        LOG;
    }

    explicit operator bool()
    {
        fill();
        return !m_H.done();
    }

    unsigned operator()()
    {
        fill();
        m_Full = false;
        return std::move(m_H.promise().m_Value);
    }

    void fill()
    {
        if (!m_Full)
        {
            m_H();
            if (m_H.promise().m_Exception)
                std::rethrow_exception(m_H.promise().m_Exception);
            m_Full = true;
        }
    }
};

Generator counter()
{
    for (unsigned i = 0; i < 3; ++i)
    {
        co_yield i;
    }
}

int main()
{
    Generator g = counter();
    while (g)
    {
        std::cout << "In main function -- " << g() << "\n";
    }
    return 0;
}