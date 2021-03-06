//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_REVERSE_COPY_HPP
#define BOOST_COMPUTE_ALGORITHM_REVERSE_COPY_HPP

#include <iterator>

#include <boost/compute/command_queue.hpp>
#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/algorithm/reverse.hpp>
#include <boost/compute/detail/default_queue_for_iterator.hpp>

namespace boost {
namespace compute {

template<class InputIterator, class OutputIterator>
inline OutputIterator reverse_copy(InputIterator first,
                                   InputIterator last,
                                   OutputIterator result,
                                   command_queue &queue)
{
    typedef typename std::iterator_traits<OutputIterator>::difference_type difference_type;

    difference_type count = std::distance(first, last);

    // copy data to result
    ::boost::compute::copy(first, last, result, queue);

    // reverse result
    ::boost::compute::reverse(result, result + count, queue);

    // return iterator to the end of result
    return result + count;
}

template<class InputIterator, class OutputIterator>
inline OutputIterator reverse_copy(InputIterator first,
                                   InputIterator last,
                                   OutputIterator result)
{
    command_queue queue = detail::default_queue_for_iterator(first);

    return ::boost::compute::reverse_copy(first, last, result, queue);
}

} // end compute namespace
} // end boost namespace

#endif // BOOST_COMPUTE_ALGORITHM_REVERSE_COPY_HPP
