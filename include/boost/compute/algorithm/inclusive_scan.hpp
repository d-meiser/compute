//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_INCLUSIVE_SCAN_HPP
#define BOOST_COMPUTE_ALGORITHM_INCLUSIVE_SCAN_HPP

#include <boost/compute/command_queue.hpp>
#include <boost/compute/algorithm/detail/scan.hpp>
#include <boost/compute/detail/default_queue_for_iterator.hpp>

namespace boost {
namespace compute {

template<class InputIterator, class OutputIterator>
inline OutputIterator inclusive_scan(InputIterator first,
                                     InputIterator last,
                                     OutputIterator result,
                                     command_queue &queue)
{
    return detail::scan(first, last, result, false, queue);
}

template<class InputIterator, class OutputIterator>
inline OutputIterator inclusive_scan(InputIterator first,
                                     InputIterator last,
                                     OutputIterator result)
{
    command_queue queue = detail::default_queue_for_iterator(result);

    return ::boost::compute::inclusive_scan(first, last, result, queue);
}

} // end compute namespace
} // end boost namespace

#endif // BOOST_COMPUTE_ALGORITHM_INCLUSIVE_SCAN_HPP
