//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#define BOOST_TEST_MODULE TestImageSampler
#include <boost/test/unit_test.hpp>

#include <boost/compute/system.hpp>
#include <boost/compute/image_sampler.hpp>

#include "context_setup.hpp"

namespace bc = boost::compute;

BOOST_AUTO_TEST_CASE(get_context)
{
    bc::image_sampler sampler(context, true, CL_ADDRESS_NONE, CL_FILTER_NEAREST);
    BOOST_CHECK(sampler.get_context() == context);
}

BOOST_AUTO_TEST_CASE(get_info)
{
    bc::image_sampler sampler(context, true, CL_ADDRESS_NONE, CL_FILTER_NEAREST);
    BOOST_CHECK_EQUAL(sampler.get_info<bool>(CL_SAMPLER_NORMALIZED_COORDS), true);
    BOOST_CHECK_EQUAL(sampler.get_info<cl_addressing_mode>(CL_SAMPLER_ADDRESSING_MODE), CL_ADDRESS_NONE);
    BOOST_CHECK_EQUAL(sampler.get_info<cl_filter_mode>(CL_SAMPLER_FILTER_MODE), CL_FILTER_NEAREST);

    sampler = bc::image_sampler(context, false, CL_ADDRESS_CLAMP, CL_FILTER_LINEAR);
    BOOST_CHECK_EQUAL(sampler.get_info<bool>(CL_SAMPLER_NORMALIZED_COORDS), false);
    BOOST_CHECK_EQUAL(sampler.get_info<cl_addressing_mode>(CL_SAMPLER_ADDRESSING_MODE), CL_ADDRESS_CLAMP);
    BOOST_CHECK_EQUAL(sampler.get_info<cl_filter_mode>(CL_SAMPLER_FILTER_MODE), CL_FILTER_LINEAR);
}

BOOST_AUTO_TEST_SUITE_END()
