//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#define BOOST_TEST_MODULE TestBuffer
#include <boost/test/unit_test.hpp>

#include <boost/compute/buffer.hpp>
#include <boost/compute/system.hpp>

#include "context_setup.hpp"

namespace bc = boost::compute;

BOOST_AUTO_TEST_CASE(size)
{
    bc::buffer buffer(context, 100);
    BOOST_CHECK_EQUAL(buffer.size(), size_t(100));
    BOOST_VERIFY(buffer.max_size() > buffer.size());
}

BOOST_AUTO_TEST_CASE(cl_context)
{
    bc::buffer buffer(context, 100);
    BOOST_VERIFY(buffer.get_context() == context);
}

BOOST_AUTO_TEST_CASE(equality_operator)
{
    bc::buffer a(context, 10);
    bc::buffer b(context, 10);
    BOOST_VERIFY(a == a);
    BOOST_VERIFY(b == b);
    BOOST_VERIFY(!(a == b));
    BOOST_VERIFY(a != b);

    a = b;
    BOOST_VERIFY(a == b);
    BOOST_VERIFY(!(a != b));
}

BOOST_AUTO_TEST_CASE(construct_from_cl_mem)
{
    // create cl_mem
    cl_mem mem = clCreateBuffer(context, CL_MEM_READ_WRITE, 16, 0, 0);
    BOOST_VERIFY(mem);

    // create boost::compute::buffer
    boost::compute::buffer buffer(mem);

    // check buffer
    BOOST_CHECK(buffer.get() == mem);
    BOOST_CHECK(buffer.get_context() == context);
    BOOST_CHECK_EQUAL(buffer.size(), size_t(16));

    // cleanup cl_mem
    clReleaseMemObject(mem);
}

BOOST_AUTO_TEST_CASE(reference_count)
{
    using boost::compute::uint_;

    boost::compute::buffer buf(context, 16);
    BOOST_CHECK_GE(buf.reference_count(), uint_(1));
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
    boost::compute::buffer buffer1(context, 16);
    BOOST_CHECK(buffer1.get() != 0);
    BOOST_CHECK_EQUAL(buffer1.size(), size_t(16));

    boost::compute::buffer buffer2(boost::move(buffer1));
    BOOST_CHECK(buffer1.get() == 0);
    BOOST_CHECK(buffer2.get() != 0);
    BOOST_CHECK_EQUAL(buffer2.size(), size_t(16));
}

BOOST_AUTO_TEST_SUITE_END()
