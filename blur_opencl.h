#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "bitmap.h"
#include "code_timer.h"
#include "utilities.h"
#include "CL/cl.h"

#define CHECK_STATUS(M) \
do{if(status!=CL_SUCCESS){std::cerr<<BRACKETED_LINE("OpenCL Error: "<<M<<" Error code "<<status);return;}} while(false)

void BlurOpenCLZeroCopy(
	const Bitmap& bitmap, 
	Bitmap& bitmap_dest, 
	const cl_device_id device_id, 
	const std::string & binary_file_name, 
	const std::string & function_name,
	const size_t * global_work_sizes,
	const size_t * local_work_sizes);

// bmp must have a width that is a multiple of 4
void BlurOpenCLImageZeroCopy(
	const Bitmap & bitmap,
	Bitmap & bitmap_dest,
	const cl_device_id device_id,
	const std::string & binary_file_name,
	const std::string & function_name,
	const size_t * global_work_sizes,
	const size_t * local_work_sizes);


void BlurOpenCL(
	const Bitmap & bitmap,
	Bitmap & bitmap_dest,
	const cl_device_id device_id,
	const std::string & binary_file_name,
	const std::string & function_name,
	const size_t * global_work_sizes,
	const size_t * local_work_sizes);

void CompileOpenCLKernelBinary(
	const cl_device_id device_id,
	const std::string& options);