//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_CONTAINER_ARRAY_HPP
#define BOOST_COMPUTE_CONTAINER_ARRAY_HPP

#include <cstddef>
#include <iterator>
#include <exception>

#include <boost/array.hpp>
#include <boost/throw_exception.hpp>

#include <boost/compute/buffer.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/algorithm/fill.hpp>
#include <boost/compute/algorithm/swap_ranges.hpp>
#include <boost/compute/iterator/buffer_iterator.hpp>
#include <boost/compute/detail/buffer_value.hpp>

namespace boost {
namespace compute {

template<class T, std::size_t N>
class array
{
public:
    typedef T value_type;
    typedef std::size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef detail::buffer_value<T> reference;
    typedef const detail::buffer_value<T> const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef buffer_iterator<T> iterator;
    typedef buffer_iterator<T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    enum {
        static_size = N
    };

    explicit array(const context &context = system::default_context())
        : m_buffer(context, sizeof(T) * N)
    {
    }

    array(const array<T, N> &other)
        : m_buffer(other.m_buffer.get_context(), sizeof(T) * N)
    {
        boost::compute::copy(other.begin(), other.end(), begin());
    }

    array(const boost::array<T, N> &array,
          const context &context = system::default_context())
        : m_buffer(context, sizeof(T) * N)
    {
        boost::compute::copy(array.begin(), array.end(), begin());
    }

    array<T, N>& operator=(const array<T, N> &other)
    {
        if(this != &other){
            boost::compute::copy(other.begin(), other.end(), begin());
        }

        return *this;
    }

    array<T, N>& operator=(const boost::array<T, N> &array)
    {
        boost::compute::copy(array.begin(), array.end(), begin());

        return *this;
    }

    ~array()
    {
    }

    iterator begin()
    {
        return buffer_iterator<T>(m_buffer, 0);
    }

    const_iterator begin() const
    {
        return buffer_iterator<T>(m_buffer, 0);
    }

    const_iterator cbegin() const
    {
        return begin();
    }

    iterator end()
    {
        return buffer_iterator<T>(m_buffer, N);
    }

    const_iterator end() const
    {
        return buffer_iterator<T>(m_buffer, N);
    }

    const_iterator cend() const
    {
        return end();
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end() - 1);
    }

    const_reverse_iterator rbegin() const
    {
        return reverse_iterator(end() - 1);
    }

    const_reverse_iterator crbegin() const
    {
        return rbegin();
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin() - 1);
    }

    const_reverse_iterator rend() const
    {
        return reverse_iterator(begin() - 1);
    }

    const_reverse_iterator crend() const
    {
        return rend();
    }

    size_type size() const
    {
        return N;
    }

    bool empty() const
    {
        return N == 0;
    }

    size_type max_size() const
    {
        return N;
    }

    reference operator[](size_type index)
    {
        return *(begin() + static_cast<difference_type>(index));
    }

    const_reference operator[](size_type index) const
    {
        return *(begin() + static_cast<difference_type>(index));
    }

    reference at(size_type index)
    {
        if(index >= N){
            BOOST_THROW_EXCEPTION(std::out_of_range("index out of range"));
        }

        return operator[](index);
    }

    const_reference at(size_type index) const
    {
        if(index >= N){
            BOOST_THROW_EXCEPTION(std::out_of_range("index out of range"));
        }

        return operator[](index);
    }

    reference front()
    {
        return *begin();
    }

    const_reference front() const
    {
        return *begin();
    }

    reference back()
    {
        return *(end() - static_cast<difference_type>(1));
    }

    const_reference back() const
    {
        return *(end() - static_cast<difference_type>(1));
    }

    void fill(const value_type &value)
    {
        ::boost::compute::fill(begin(), end(), value);
    }

    void swap(array<T, N> &other)
    {
        ::boost::compute::swap_ranges(begin(), end(), other.begin());
    }

    const buffer& get_buffer() const
    {
        return m_buffer;
    }

private:
    buffer m_buffer;
};

} // end compute namespace
} // end boost namespace

#endif // BOOST_COMPUTE_CONTAINER_ARRAY_HPP
