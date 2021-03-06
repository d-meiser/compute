//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_REPLACE_COPY_HPP
#define BOOST_COMPUTE_ALGORITHM_REPLACE_COPY_HPP

#include <iterator>

#include <boost/compute/command_queue.hpp>
#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/algorithm/replace.hpp>
#include <boost/compute/detail/default_queue_for_iterator.hpp>

namespace boost {
namespace compute {

template<class InputIterator, class OutputIterator, class T>
inline OutputIterator replace_copy(InputIterator first,
                                   InputIterator last,
                                   OutputIterator result,
                                   const T &old_value,
                                   const T &new_value,
                                   command_queue &queue)
{
    typedef typename std::iterator_traits<OutputIterator>::difference_type difference_type;

    difference_type count = std::distance(first, last);
    if(count == 0){
        return result;
    }

    // copy data to result
    ::boost::compute::copy(first, last, result, queue);

    // replace in result
    ::boost::compute::replace(result,
                              result + count,
                              old_value,
                              new_value,
                              queue);

    // return iterator to the end of result
    return result + count;
}

template<class InputIterator, class OutputIterator, class T>
inline OutputIterator replace_copy(InputIterator first,
                                   InputIterator last,
                                   OutputIterator result,
                                   const T &old_value,
                                   const T &new_value)
{
    command_queue queue = detail::default_queue_for_iterator(first);

    return ::boost::compute::replace_copy(first,
                                          last,
                                          result,
                                          old_value,
                                          new_value,
                                          queue);
}

} // end compute namespace
} // end boost namespace

#endif // BOOST_COMPUTE_ALGORITHM_REPLACE_COPY_HPP
