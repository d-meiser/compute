//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_PROGRAM_HPP
#define BOOST_COMPUTE_PROGRAM_HPP

#include <string>
#include <vector>
#include <fstream>
#include <streambuf>

#ifdef BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION
#include <iostream>
#endif

#include <boost/move/move.hpp>

#include <boost/compute/cl.hpp>
#include <boost/compute/context.hpp>
#include <boost/compute/exception.hpp>
#include <boost/compute/detail/program_create_kernel_result.hpp>

namespace boost {
namespace compute {

class program
{
public:
    program()
        : m_program(0)
    {
    }

    explicit program(cl_program program, bool retain = true)
        : m_program(program)
    {
        if(m_program && retain){
            clRetainProgram(m_program);
        }
    }

    program(const program &other)
        : m_program(other.m_program)
    {
        if(m_program){
            clRetainProgram(m_program);
        }
    }

    program(BOOST_RV_REF(program) other)
        : m_program(other.m_program)
    {
        other.m_program = 0;
    }

    program& operator=(const program &other)
    {
        if(this != &other){
            if(m_program){
                clReleaseProgram(m_program);
            }

            m_program = other.m_program;

            if(m_program){
                clRetainProgram(m_program);
            }
        }

        return *this;
    }

    program& operator=(BOOST_RV_REF(program) other)
    {
        if(this != &other){
            if(m_program){
                clReleaseProgram(m_program);
            }

            m_program = other.m_program;
            other.m_program = 0;
        }

        return *this;
    }

    ~program()
    {
        if(m_program){
            clReleaseProgram(m_program);
        }
    }

    cl_program& get() const
    {
        return const_cast<cl_program &>(m_program);
    }

    std::string source() const
    {
        return get_info<std::string>(CL_PROGRAM_SOURCE);
    }

    std::vector<unsigned char> binary() const
    {
        size_t binary_size = get_info<size_t>(CL_PROGRAM_BINARY_SIZES);
        std::vector<unsigned char> binary(binary_size);

        unsigned char *binary_ptr = &binary[0];
        cl_int error = clGetProgramInfo(m_program,
                                        CL_PROGRAM_BINARIES,
                                        sizeof(unsigned char **),
                                        &binary_ptr,
                                        0);
        if(error != CL_SUCCESS){
            BOOST_THROW_EXCEPTION(runtime_exception(error));
        }

        return binary;
    }

    std::vector<device> get_devices() const
    {
        size_t device_ids_size = 0;
        clGetProgramInfo(m_program,
                         CL_PROGRAM_DEVICES,
                         0,
                         0,
                         &device_ids_size);

        std::vector<cl_device_id> device_ids(device_ids_size / sizeof(size_t));
        clGetProgramInfo(m_program,
                         CL_PROGRAM_DEVICES,
                         device_ids_size,
                         &device_ids[0],
                         0);

        std::vector<device> devices;
        for(size_t i = 0; i < device_ids.size(); i++){
            devices.push_back(device(device_ids[i]));
        }

        return devices;
    }

    context get_context() const
    {
        return context(get_info<cl_context>(CL_PROGRAM_CONTEXT));
    }

    template<class T>
    T get_info(cl_program_info info) const
    {
        return detail::get_object_info<T>(clGetProgramInfo, m_program, info);
    }

    cl_int build(const std::string &options = std::string())
    {
        const char *options_string = 0;

        if(!options.empty()){
            options_string = options.c_str();
        }

        cl_int ret = clBuildProgram(m_program, 0, 0, options_string, 0, 0);

        #ifdef BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION
        if(ret != CL_SUCCESS){
            // print the error, source code and build log
            std::cerr << "Boost.Compute: "
                      << "kernel compilation failed (" << ret << ")\n"
                      << "--- source ---\n"
                      << source()
                      << "\n--- build log ---\n"
                      << build_log()
                      << std::endl;
        }
        #endif

        if(ret != CL_SUCCESS){
            BOOST_THROW_EXCEPTION(runtime_exception(ret));
        }

        return ret;
    }

    std::string build_log() const
    {
        device device = get_devices()[0];

        size_t size = 0;
        cl_int ret = clGetProgramBuildInfo(m_program,
                                           device.id(),
                                           CL_PROGRAM_BUILD_LOG,
                                           0,
                                           0,
                                           &size);
        if(ret != CL_SUCCESS){
            BOOST_THROW_EXCEPTION(runtime_exception(ret));
        }

        std::string value(size - 1, 0);
        ret = clGetProgramBuildInfo(m_program,
                                    device.id(),
                                    CL_PROGRAM_BUILD_LOG,
                                    size,
                                    &value[0],
                                    0);
        if(ret != CL_SUCCESS){
            BOOST_THROW_EXCEPTION(runtime_exception(ret));
        }

        return value;
    }

    detail::program_create_kernel_result
    create_kernel(const std::string &name) const
    {
        cl_int error = 0;
        cl_kernel kernel = clCreateKernel(m_program,
                                          name.c_str(),
                                          &error);
        if(!kernel){
            BOOST_THROW_EXCEPTION(runtime_exception(error));
        }

        return detail::program_create_kernel_result(kernel);
    }

    operator cl_program() const
    {
        return m_program;
    }

    static program create_with_source(const std::string &source,
                                      const context &context)
    {
        const char *source_string = source.c_str();

        cl_int error = 0;
        cl_program program_ = clCreateProgramWithSource(context,
                                                        uint_(1),
                                                        &source_string,
                                                        0,
                                                        &error);
        if(!program_){
            BOOST_THROW_EXCEPTION(runtime_exception(error));
        }

        return program(program_, false);
    }

    static program create_with_source_file(const std::string &file,
                                           const context &context)
    {
        // open file stream
        std::ifstream stream(file.c_str());

        // read source
        std::string source(
            (std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>()
        );

        // create program
        return create_with_source(source, context);
    }

    static program create_with_binary(const unsigned char *binary,
                                      size_t binary_size,
                                      const context &context)
    {
        const cl_device_id device = context.get_device().id();

        cl_int error = 0;
        cl_int binary_status = 0;
        cl_program program_ = clCreateProgramWithBinary(context,
                                                        uint_(1),
                                                        &device,
                                                        &binary_size,
                                                        &binary,
                                                        &binary_status,
                                                        &error);
        if(!program_){
            BOOST_THROW_EXCEPTION(runtime_exception(error));
        }

        return program(program_, false);
    }

    static program create_with_binary(const std::vector<unsigned char> &binary,
                                      const context &context)
    {
        return create_with_binary(&binary[0], binary.size(), context);
    }

    static program create_with_binary_file(const std::string &file,
                                           const context &context)
    {
        // open file stream
        std::ifstream stream(file.c_str(), std::ios::in | std::ios::binary);

        // read binary
        std::vector<unsigned char> binary(
            (std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>()
        );

        // create program
        return create_with_binary(&binary[0], binary.size(), context);
    }

private:
    BOOST_COPYABLE_AND_MOVABLE(program)

    cl_program m_program;
};

} // end compute namespace
} // end boost namespace

#endif // BOOST_COMPUTE_PROGRAM_HPP
