//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_COPY_HPP
#define BOOST_COMPUTE_ALGORITHM_COPY_HPP

#include <algorithm>
#include <iterator>

#include <boost/utility/enable_if.hpp>

#include <boost/compute/buffer.hpp>
#include <boost/compute/future.hpp>
#include <boost/compute/command_queue.hpp>
#include <boost/compute/iterator/buffer_iterator.hpp>
#include <boost/compute/detail/is_device_iterator.hpp>
#include <boost/compute/detail/is_contiguous_iterator.hpp>
#include <boost/compute/detail/iterator_range_size.hpp>
#include <boost/compute/algorithm/detail/copy_to_host.hpp>
#include <boost/compute/algorithm/detail/copy_on_device.hpp>
#include <boost/compute/algorithm/detail/copy_to_device.hpp>
#include <boost/compute/algorithm/detail/default_queue_for_copy.hpp>

namespace boost {
namespace compute {
namespace detail {

// host -> device
template<class InputIterator, class OutputIterator>
inline OutputIterator
dispatch_copy(InputIterator first,
              InputIterator last,
              OutputIterator result,
              command_queue &queue,
              typename boost::enable_if_c<
                  !is_device_iterator<InputIterator>::value &&
                  is_device_iterator<OutputIterator>::value
              >::type* = 0)
{
    if(is_contiguous_iterator<InputIterator>::value){
        return copy_to_device(first, last, result, queue);
    }
    else {
        // for non-contiguous input we first copy the values to
        // a temporary std::vector and then copy from there
        typedef typename std::iterator_traits<InputIterator>::value_type T;
        std::vector<T> vector(first, last);
        return copy_to_device(vector.begin(), vector.end(), result, queue);
    }
}

// host -> device (async)
template<class InputIterator, class OutputIterator>
inline future<OutputIterator>
dispatch_copy_async(InputIterator first,
                    InputIterator last,
                    OutputIterator result,
                    command_queue &queue,
                    typename boost::enable_if_c<
                        !is_device_iterator<InputIterator>::value &&
                        is_device_iterator<OutputIterator>::value
                    >::type* = 0)
{
    if(is_contiguous_iterator<InputIterator>::value){
        return copy_to_device_async(first, last, result, queue);
    }
    else {
        BOOST_ASSERT(false && "copy_async() is not supported for non-contiguous iterators");
        return future<OutputIterator>();
    }
}

// device -> host
template<class InputIterator, class OutputIterator>
inline OutputIterator
dispatch_copy(InputIterator first,
              InputIterator last,
              OutputIterator result,
              command_queue &queue,
              typename boost::enable_if_c<
                  is_device_iterator<InputIterator>::value &&
                  !is_device_iterator<OutputIterator>::value
              >::type* = 0)
{
    if(is_contiguous_iterator<OutputIterator>::value){
        return copy_to_host(first, last, result, queue);
    }
    else {
        // for non-contiguous input we first copy the values to
        // a temporary std::vector and then copy from there
        typedef typename std::iterator_traits<InputIterator>::value_type T;
        std::vector<T> vector(iterator_range_size(first, last));
        copy_to_host(first, last, vector.begin(), queue);
        return std::copy(vector.begin(), vector.end(), result);
    }
}

// device -> host (async)
template<class InputIterator, class OutputIterator>
inline future<OutputIterator>
dispatch_copy_async(InputIterator first,
                    InputIterator last,
                    OutputIterator result,
                    command_queue &queue,
                    typename boost::enable_if_c<
                        is_device_iterator<InputIterator>::value &&
                       !is_device_iterator<OutputIterator>::value
                    >::type* = 0)
{
    if(is_contiguous_iterator<OutputIterator>::value){
        return copy_to_host_async(first, last, result, queue);
    }
    else {
        BOOST_ASSERT(false && "copy_async() is not supported for non-contiguous iterators");
        return future<OutputIterator>();
    }
}

// device -> device
template<class InputIterator, class OutputIterator>
inline OutputIterator
dispatch_copy(InputIterator first,
              InputIterator last,
              OutputIterator result,
              command_queue &queue,
              typename boost::enable_if_c<
                  is_device_iterator<InputIterator>::value &&
                  is_device_iterator<OutputIterator>::value &&
                  !(boost::is_same<
                        InputIterator,
                        buffer_iterator<typename InputIterator::value_type>
                        >::value &&
                    boost::is_same<
                        OutputIterator,
                        buffer_iterator<typename OutputIterator::value_type>
                    >::value &&
                    boost::is_same<
                        typename InputIterator::value_type,
                        typename OutputIterator::value_type
                    >::value)
              >::type* = 0)
{
    return copy_on_device(first, last, result, queue);
}

// device -> device (specialization for buffer iterators)
template<class InputIterator, class OutputIterator>
inline OutputIterator
dispatch_copy(InputIterator first,
              InputIterator last,
              OutputIterator result,
              command_queue &queue,
              typename boost::enable_if_c<
                  boost::is_same<
                      InputIterator,
                      buffer_iterator<typename InputIterator::value_type>
                  >::value &&
                  boost::is_same<
                      OutputIterator,
                      buffer_iterator<typename OutputIterator::value_type>
                  >::value &&
                  boost::is_same<
                      typename InputIterator::value_type,
                      typename OutputIterator::value_type
                  >::value
              >::type* = 0)
{
    typedef typename std::iterator_traits<InputIterator>::value_type value_type;
    typedef typename std::iterator_traits<InputIterator>::difference_type difference_type;

    difference_type n = std::distance(first, last);
    if(n < 1){
        // nothing to copy
        return first;
    }

    queue.enqueue_copy_buffer(first.get_buffer(),
                              result.get_buffer(),
                              first.get_index() * sizeof(value_type),
                              result.get_index() * sizeof(value_type),
                              static_cast<size_t>(n) * sizeof(value_type));
    return result + n;
}

} // end detail namespace

template<class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first,
                           InputIterator last,
                           OutputIterator result,
                           command_queue &queue)
{
    return detail::dispatch_copy(first, last, result, queue);
}

template<class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first,
                           InputIterator last,
                           OutputIterator result)
{
    command_queue queue = detail::default_queue_for_copy(first, result);

    return copy(first, last, result, queue);
}

template<class InputIterator, class OutputIterator>
inline future<OutputIterator> copy_async(InputIterator first,
                                         InputIterator last,
                                         OutputIterator result,
                                         command_queue &queue)
{
    return detail::dispatch_copy_async(first, last, result, queue);
}

} // end compute namespace
} // end boost namespace

#endif // BOOST_COMPUTE_ALGORITHM_COPY_HPP
