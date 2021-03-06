//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_EQUAL_RANGE_HPP
#define BOOST_COMPUTE_ALGORITHM_EQUAL_RANGE_HPP

#include <utility>

#include <boost/compute/command_queue.hpp>
#include <boost/compute/algorithm/lower_bound.hpp>
#include <boost/compute/algorithm/upper_bound.hpp>
#include <boost/compute/detail/default_queue_for_iterator.hpp>

namespace boost {
namespace compute {

template<class InputIterator, class T>
inline std::pair<InputIterator, InputIterator>
equal_range(InputIterator first,
            InputIterator last,
            const T &value,
            command_queue &queue)
{
    return std::make_pair(
               ::boost::compute::lower_bound(first, last, value, queue),
               ::boost::compute::upper_bound(first, last, value, queue)
           );
}

template<class InputIterator, class T>
inline std::pair<InputIterator, InputIterator>
equal_range(InputIterator first,
            InputIterator last,
            const T &value)
{
    command_queue queue = detail::default_queue_for_iterator(first);

    return ::boost::compute::equal_range(first, last, value, queue);
}

} // end compute namespace
} // end boost namespace

#endif // BOOST_COMPUTE_ALGORITHM_EQUAL_RANGE_HPP
