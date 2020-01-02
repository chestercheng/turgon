#pragma once

/*
 * Copyright (c) 2020, Yung-Yu Chen <yyc@solvcon.net>
 * BSD 3-Clause License, see LICENSE.txt
 */

#include <stdexcept>
#include <array>

namespace spacetime
{

template < typename T, size_t N=3 >
class small_vector
{

public:

    using value_type = T;
    using iterator = T *;
    using const_iterator = T const *;

    explicit small_vector(size_t size)
      : m_size(size)
    {
        if (m_size > N)
        {
            m_capacity = m_size;
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            m_head = new T[m_capacity];
        }
        else
        {
            m_capacity = N;
            m_head = m_data.data();
        }
    }

    small_vector(std::initializer_list<T> init)
      : small_vector(init.size())
    {
        std::copy_n(init.begin(), m_size, begin());
    }

    explicit small_vector(std::vector<T> const & vector)
      : small_vector(vector.size())
    {
        std::copy_n(vector.begin(), m_size, begin());
    }

    small_vector() { m_head = m_data.data(); }

    small_vector(small_vector const & other)
      : m_size(other.m_size)
    {
        if (other.m_head == other.m_data.data())
        {
            m_capacity = N;
            m_head = m_data.data();
        }
        else
        {
            m_capacity = m_size;
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            m_head = new T[m_capacity];
        }
        std::copy_n(other.m_head, m_size, m_head);
    }

    small_vector(small_vector && other) noexcept
      : m_size(other.m_size)
    {
        if (other.m_head == other.m_data.data())
        {
            m_capacity = N;
            std::copy_n(other.m_data.begin(), m_size, m_data.begin());
            m_head = m_data.data();
        }
        else
        {
            m_capacity = m_size;
            m_head = other.m_head;
            other.m_size = 0;
            other.m_capacity = N;
            other.m_head = other.m_data.data();
        }
    }

    small_vector & operator=(small_vector const & other)
    {
        if (this != &other)
        {
            if (other.m_head == other.m_data.data())
            {
                if (m_head != m_data.data())
                {
                    delete[] m_head;
                    m_head = m_data.data();
                }
                m_size = other.m_size;
                m_capacity = N;
            }
            else
            {
                if (m_capacity < other.m_size)
                {
                    delete[] m_head;
                    m_head = nullptr;
                }
                if (m_head == m_data.data() || m_head == nullptr)
                {
                    m_capacity = other.m_size;
                    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
                    m_head = new T[m_capacity];
                }
                m_size = other.m_size;
            }
            std::copy_n(other.m_head, m_size, m_head);
        }
        return *this;
    }

    small_vector & operator=(small_vector && other) noexcept
    {
        if (this != &other)
        {
            if (other.m_head == other.m_data.data())
            {
                if (m_head != m_data.data())
                {
                    delete[] m_head;
                    m_head = m_data.data();
                }
                m_size = other.m_size;
                m_capacity = N;
                std::copy_n(other.m_data.begin(), m_size, m_data.begin());
            }
            else
            {
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                m_head = other.m_head;
                other.m_size = 0;
                other.m_capacity = N;
                other.m_head = other.m_data.data();
           }
        }
        return *this;
    }

    ~small_vector()
    {
        if (m_head != m_data.data() && m_head != nullptr)
        {
            delete[] m_head;
            m_head = nullptr;
        }
    }

    size_t size() const noexcept { return m_size; }
    size_t capacity() const noexcept { return m_capacity; }
    bool empty() const noexcept { return 0 == m_size; }

    iterator begin() noexcept { return m_head; }
    iterator end() noexcept { return m_head + m_size; }
    const_iterator begin() const noexcept { return m_head; }
    const_iterator end() const noexcept { return m_head + m_size; }
    const_iterator cbegin() const noexcept { return m_head; }
    const_iterator cend() const noexcept { return m_head + m_size; }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    T const & operator[](size_t it) const { return m_head[it]; }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    T       & operator[](size_t it)       { return m_head[it]; }

    T const & at(size_t it) const { validate_range(it); return (*this)[it]; }
    T       & at(size_t it)       { validate_range(it); return (*this)[it]; }

    T const * data() const { return m_head; }
    T       * data()       { return m_head; }

    void clear() noexcept
    {
        if (m_head != m_data.data())
        {
            delete[] m_head;
            m_head = m_data.data();
        }
        m_size = 0;
        m_capacity = N;
    }

    void push_back(T && value)
    {
        if (m_size == m_capacity)
        {
            m_capacity *= 2;
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            T * storage = new T[m_capacity];
            std::copy_n(m_head, m_size, storage);
            if (m_head != m_data.data())
            {
                delete[] m_head;
            }
            m_head = storage;
        }
        m_head[m_size++] = value;
    }

private:

    void validate_range(size_t it) const
    {
        if (it >= size())
        {
            throw std::out_of_range("small_vector: index out of range");
        }
    }

    T * m_head = nullptr;
    unsigned int m_size = 0;
    unsigned int m_capacity = N;
    std::array<T,N> m_data;

}; /* end class small_vector */

template < typename T >
bool operator==(small_vector<T> const & lhs, small_vector<T> const & rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

static_assert(sizeof(small_vector<size_t>) == 40, "small_vector<size_t> should use 40 bytes");

} /* end namespace spacetime */

/* vim: set et ts=4 sw=4: */
