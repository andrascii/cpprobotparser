#pragma once

namespace cpprobotparser
{

template <typename T>
class Pimpl final
{
public:
    Pimpl()
        : m_impl(new T)
    {
    }
    Pimpl(const Pimpl& other)
        : m_impl(new T(*other.m_impl))
    {
    }
    Pimpl(Pimpl&& other)
        : m_impl(other.m_impl)
    {
        other.m_impl = nullptr;
    }

    ~Pimpl()
    {
        delete m_impl;
    }

    Pimpl& operator=(const Pimpl& other)
    {
        if (this == std::addressof(other))
        {
            return *this;
        }

        *m_impl = *other.m_impl;
        return *this;
    }
    Pimpl& operator=(Pimpl&& other)
    {
        if (this == std::addressof(other))
        {
            return *this;
        }

        delete m_impl;
        m_impl = other.m_impl;
        other.m_impl = nullptr;
        return *this;
    }

    const T* operator->() const noexcept
    {
        return m_impl;
    }
    T* operator->() noexcept
    {
        return m_impl;
    }

    const T* get() const noexcept
    {
        return m_impl;
    }
    T* get() noexcept
    {
        return m_impl;
    }

private:
    T* m_impl;
};

}