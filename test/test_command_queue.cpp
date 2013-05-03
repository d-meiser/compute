//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#define BOOST_TEST_MODULE TestCommandQueue
#include <boost/test/unit_test.hpp>

#include <boost/compute/kernel.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/program.hpp>
#include <boost/compute/command_queue.hpp>

#include "context_setup.hpp"

namespace bc = boost::compute;

BOOST_AUTO_TEST_CASE(get_context)
{
    BOOST_VERIFY(queue.get_context() == context);
}

BOOST_AUTO_TEST_CASE(event_profiling)
{
    bc::command_queue queue(context, device, bc::command_queue::enable_profiling);

    int data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    bc::buffer buffer(context, sizeof(data));

    bc::event event =
        queue.enqueue_write_buffer_async(buffer,
                                         0,
                                         sizeof(data),
                                         static_cast<const void *>(data));
    queue.finish();

    event.get_profiling_info<cl_ulong>(bc::event::profiling_command_queued);
    event.get_profiling_info<cl_ulong>(bc::event::profiling_command_submit);
    event.get_profiling_info<cl_ulong>(bc::event::profiling_command_start);
    event.get_profiling_info<cl_ulong>(bc::event::profiling_command_end);
}

BOOST_AUTO_TEST_CASE(kernel_profiling)
{
    // create queue with profiling enabled
    boost::compute::command_queue queue(
        context, device, boost::compute::command_queue::enable_profiling
    );

    // input data
    int data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    boost::compute::buffer buffer(context, sizeof(data));

    // copy input data to device
    queue.enqueue_write_buffer(buffer, sizeof(data), data);

    // setup kernel
    const char source[] =
        "__kernel void iscal(__global int *buffer, int alpha)\n"
        "{\n"
        "    buffer[get_global_id(0)] *= alpha;\n"
        "}\n";

    boost::compute::program program =
        boost::compute::program::create_with_source(source, context);
    program.build();

    boost::compute::kernel kernel(program, "iscal");
    kernel.set_arg(0, buffer);
    kernel.set_arg(1, 2);

    // execute kernel
    size_t global_work_offset = 0;
    size_t global_work_size = 8;

    boost::compute::event event =
        queue.enqueue_nd_range_kernel(kernel,
                                      size_t(1),
                                      &global_work_offset,
                                      &global_work_size,
                                      0);

    // wait until kernel is finished
    event.wait();

    // check profiling information
    event.get_profiling_info<cl_ulong>(bc::event::profiling_command_queued);
    event.get_profiling_info<cl_ulong>(bc::event::profiling_command_submit);
    event.get_profiling_info<cl_ulong>(bc::event::profiling_command_start);
    event.get_profiling_info<cl_ulong>(bc::event::profiling_command_end);

    // read results back to host
    queue.enqueue_read_buffer(buffer, sizeof(data), data);

    // check results
    BOOST_CHECK_EQUAL(data[0], 2);
    BOOST_CHECK_EQUAL(data[1], 4);
    BOOST_CHECK_EQUAL(data[2], 6);
    BOOST_CHECK_EQUAL(data[3], 8);
    BOOST_CHECK_EQUAL(data[4], 10);
    BOOST_CHECK_EQUAL(data[5], 12);
    BOOST_CHECK_EQUAL(data[6], 14);
    BOOST_CHECK_EQUAL(data[7], 16);
}

BOOST_AUTO_TEST_CASE(construct_from_cl_command_queue)
{
    // create cl_command_queue
    cl_command_queue cl_queue =
      clCreateCommandQueue(context, device.id(), 0, 0);
    BOOST_VERIFY(cl_queue);

    // create boost::compute::command_queue
    boost::compute::command_queue queue(cl_queue);

    // check queue
    BOOST_CHECK(queue.get_context() == context);
    BOOST_CHECK(cl_command_queue(queue) == cl_queue);

    // cleanup cl_command_queue
    clReleaseCommandQueue(cl_queue);
}

#ifdef CL_VERSION_1_1
BOOST_AUTO_TEST_CASE(write_buffer_rect)
{
    int data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    boost::compute::buffer buffer(context, 8 * sizeof(int));

    // copy every other value to the buffer
    size_t buffer_origin[] = { 0, 0, 0 };
    size_t host_origin[] = { 0, 0, 0 };
    size_t region[] = { sizeof(int), sizeof(int), 1 };

    queue.enqueue_write_buffer_rect(
        buffer,
        buffer_origin,
        host_origin,
        region,
        sizeof(int),
        0,
        2 * sizeof(int),
        0,
        data
    );

    // check output values
    int output[4];
    queue.enqueue_read_buffer(buffer, 4 * sizeof(int), output);
    BOOST_CHECK_EQUAL(output[0], 1);
    BOOST_CHECK_EQUAL(output[1], 3);
    BOOST_CHECK_EQUAL(output[2], 5);
    BOOST_CHECK_EQUAL(output[3], 7);
}
#endif // CL_VERSION_1_1

BOOST_AUTO_TEST_SUITE_END()
