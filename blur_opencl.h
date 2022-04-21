#pragma once

#pragma warning(disable:4996)

#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "macros.h"
#include "bitmap.h"
#include "code_timer.h"
#include "utilities.h"
#include "CL/cl.h"

#define CHECK_STATUS(M) \
do{if(status!=CL_SUCCESS){std::cerr<<BRACKETED_LINE("OpenCL Error: "<<M<<" Error code "<<status);return;}} while(false)


class BlurOpenCL
{
public:
	static void BlurOpenCLZeroCopy(
		const Bitmap& bitmap,
		Bitmap& bitmap_dest,
		const cl_device_id device_id,
		const std::string& binary_file_name,
		const std::string& function_name,
		const size_t* global_work_sizes,
		const size_t* local_work_sizes,
		double& nd_range_time);


	// bmp must have a width that is a multiple of 4
	static void BlurOpenCLImageZeroCopy(
		const Bitmap& bitmap,
		Bitmap& bitmap_dest,
		const cl_device_id device_id,
		const std::string& binary_file_name,
		const std::string& function_name,
		const size_t* global_work_sizes,
		const size_t* local_work_sizes,
		double& nd_range_time);


	// we encounter errors when using BlurOpenCLImageZeroCopy for gfx803.
	// when setting CL_MEM_USE_HOST_PTR to CL_MEM_COPY_HOST_PTR, BlurOpenCLImageZeroCopy
	// will not give errors but the result is a mess. 
	// This version uses OpenCL 1.1 clCreateImage2D API. This works perfectly with gfx803.
	// When using this version with Intel UHD620, it will be very slow regardless of CL_MEM_USE_HOST_PTR
	// or CL_MEM_COPY_HOST_PTR because there will be copy tasks. Compared to CL_MEM_COPY_HOST_PTR, 
	// using CL_MEM_USE_HOST_PTR only slightly decreases "Begin to clSetKernelArg" time
	// and "clEnqueueReadImage to end" time, and almost has no effect on clEnqueueNDRangeKernel time.
	static void BlurOpenCL11ImageCopyHostPtr(
		const Bitmap& bitmap,
		Bitmap& bitmap_dest,
		const cl_device_id device_id,
		const std::string& binary_file_name,
		const std::string& function_name,
		const size_t* global_work_sizes,
		const size_t* local_work_sizes,
		double& nd_range_time);


	static void BlurOpenCLCopyHostPtr(
		const Bitmap& bitmap,
		Bitmap& bitmap_dest,
		const cl_device_id device_id,
		const std::string& binary_file_name,
		const std::string& function_name,
		const size_t* global_work_sizes,
		const size_t* local_work_sizes,
		double& nd_range_time);
};