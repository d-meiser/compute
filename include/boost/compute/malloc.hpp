//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_MALLOC_HPP
#define BOOST_COMPUTE_MALLOC_HPP

#include <boost/compute/buffer.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/context.hpp>
#include <boost/compute/device_ptr.hpp>

namespace boost {
namespace compute {

template<class T>
inline device_ptr<T>
malloc(std::size_t size, const context &context = system::default_context())
{
    return device_ptr<T>(buffer(context, size * sizeof(T)));
}

inline device_ptr<char>
malloc(std::size_t size, const context &context = system::default_context())
{
    return malloc<char>(size, context);
}

template<class T>
inline void free(device_ptr<T> &ptr)
{
    const buffer *buffer = ptr.get_buffer_ptr();
    if(buffer){
        delete buffer;
    }

    ptr = device_ptr<T>();
}

} // end compute namespace
} // end boost namespace

#endif // BOOST_COMPUTE_MALLOC_HPP
