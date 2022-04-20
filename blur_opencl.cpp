#include "blur_opencl.h"

//#define USE_TIMER
#define RETURN_ND_RANGE_TIME

void BlurOpenCL::BlurOpenCLZeroCopy(
	const Bitmap& bitmap,
	Bitmap& bitmap_dest,
	const cl_device_id device_id, 
	const std::string& binary_file_name,
	const std::string& function_name,
	const size_t* global_work_sizes,
	const size_t* local_work_sizes,
	double& nd_range_time)
{
#ifdef USE_TIMER
	CodeTimer timer;
	timer.Start("Begin to clSetKernelArg");
#endif
	cl_int status = CL_SUCCESS;

	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &status);
	CHECK_STATUS("Failed to create context.");

	cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &status);
	CHECK_STATUS("Failed to create command queue.");

	std::ifstream ifs(binary_file_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
	{
		std::cerr << BRACKETED_LINE("Failed to open binary program: " << binary_file_name);
		return;
	}

	ifs.seekg(0, std::ios::end);
	size_t binary_file_sizes[] = { ifs.tellg() };
	ifs.seekg(0, std::ios::beg);

	std::vector<unsigned char> program_binary(binary_file_sizes[0]);
	ifs.read((char*)program_binary.data(), binary_file_sizes[0]);

	ifs.close();

	const unsigned char* program_binaries[] = { program_binary.data() };

	cl_program program = clCreateProgramWithBinary(
		context, 1, &device_id, binary_file_sizes, program_binaries, NULL, &status);
	CHECK_STATUS("Failed to create program with binary.");

	status = clBuildProgram(program, 1, &device_id, "", NULL, NULL);
	CHECK_STATUS("Failed to build program.");

	cl_mem source_pixel_data_buffer = clCreateBuffer(
		context, 
		CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
		bitmap.pixel_data_alloc_size(), 
		bitmap.pixel_data(), 
		&status);
	CHECK_STATUS("Failed to create source bitmap pixel data buffer.");

	cl_mem dest_pixel_data_buffer = clCreateBuffer(
		context,
		CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
		bitmap_dest.pixel_data_alloc_size(),
		bitmap_dest.pixel_data(),
		&status);
	CHECK_STATUS("Failed to create dest bitmap pixel data buffer.");

	cl_kernel kernel = clCreateKernel(program, function_name.data(), &status);
	CHECK_STATUS("Failed to create kernel.");

	int32_t bmp_width = bitmap.width_px();
	int32_t bmp_height = bitmap.height_px();

	status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &source_pixel_data_buffer);
	status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &dest_pixel_data_buffer);
	status |= clSetKernelArg(kernel, 2, sizeof(int32_t), &bmp_width);
	status |= clSetKernelArg(kernel, 3, sizeof(int32_t), &bmp_height);
	CHECK_STATUS("Failed to set kernel args.");

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clEnqueueNDRangeKernel");
#endif

	cl_event event;

#ifdef RETURN_ND_RANGE_TIME
	// use primitive method to minimize performance cost
	auto time_start = clock();
#endif

	status = clEnqueueNDRangeKernel(
		command_queue, kernel, 2, NULL, global_work_sizes, local_work_sizes, 0, NULL, &event);
	CHECK_STATUS("Failed to enqueue ND range kernel.");

	clWaitForEvents(1, &event);

#ifdef RETURN_ND_RANGE_TIME
	nd_range_time = clock() - time_start;
#endif

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clReleasexxx to end");
#endif

	clReleaseMemObject(source_pixel_data_buffer);
	clReleaseMemObject(dest_pixel_data_buffer);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);

#ifdef USE_TIMER
	timer.StopAndPrint();
#endif
}



void BlurOpenCL::BlurOpenCLImageZeroCopy(
	const Bitmap& bitmap,
	Bitmap& bitmap_dest,
	const cl_device_id device_id,
	const std::string& binary_file_name,
	const std::string& function_name,
	const size_t* global_work_sizes,
	const size_t* local_work_sizes,
	double& nd_range_time)
{
#ifdef USE_TIMER
	CodeTimer timer;
	timer.Start("Begin to clSetKernelArg");
#endif
	cl_int status = CL_SUCCESS;

	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &status);
	CHECK_STATUS("Failed to create context.");

	cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &status);
	CHECK_STATUS("Failed to create command queue.");

	std::ifstream ifs(binary_file_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
	{
		std::cerr << BRACKETED_LINE("Failed to open binary program: " << binary_file_name);
		return;
	}

	ifs.seekg(0, std::ios::end);
	size_t binary_file_sizes[] = { ifs.tellg() };
	ifs.seekg(0, std::ios::beg);

	std::vector<unsigned char> program_binary(binary_file_sizes[0]);
	ifs.read((char*)program_binary.data(), binary_file_sizes[0]);

	ifs.close();

	const unsigned char* program_binaries[] = { program_binary.data() };

	cl_program program = clCreateProgramWithBinary(
		context, 1, &device_id, binary_file_sizes, program_binaries, NULL, &status);
	CHECK_STATUS("Failed to create program with binary.");

	status = clBuildProgram(program, 1, &device_id, "", NULL, NULL);
	CHECK_STATUS("Failed to build program.");

	cl_mem source_pixel_data_buffer = clCreateBuffer(
		context,
		CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
		bitmap.pixel_data_alloc_size(),
		bitmap.pixel_data(),
		&status);
	CHECK_STATUS("Failed to create source bitmap pixel data buffer.");

	cl_mem dest_pixel_data_buffer = clCreateBuffer(
		context,
		CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
		bitmap_dest.pixel_data_alloc_size(),
		bitmap_dest.pixel_data(),
		&status);
	CHECK_STATUS("Failed to create dest bitmap pixel data buffer.");

	cl_image_format image_format{
		CL_RGBA,
		CL_UNSIGNED_INT8
	};

	// to use RGBA, we reinterpret the BGR bmp as RGBA
	cl_image_desc source_image_desc{
		CL_MEM_OBJECT_IMAGE2D,
		bitmap.width_px()*3/4,
		bitmap.height_px(),
		0,
		0,
		0,
		0,
		0,
		0,
		source_pixel_data_buffer
	};

	cl_image_desc dest_image_desc{
		CL_MEM_OBJECT_IMAGE2D,
		bitmap.width_px() * 3 / 4,
		bitmap.height_px(),
		0,
		0,
		0,
		0,
		0,
		0,
		dest_pixel_data_buffer
	};

	cl_mem source_image = clCreateImage(
		context,
		CL_MEM_READ_ONLY,
		&image_format,
		&source_image_desc,
		NULL,
		&status);
	CHECK_STATUS("Failed to create source bitmap image.");

	cl_mem dest_image = clCreateImage(
		context,
		CL_MEM_WRITE_ONLY,
		&image_format,
		&dest_image_desc,
		NULL,
		&status);
	CHECK_STATUS("Failed to create dest image.");

	cl_kernel kernel = clCreateKernel(program, function_name.data(), &status);
	CHECK_STATUS("Failed to create kernel.");

	int32_t bmp_width_reinterpreted = bitmap.width_px() * 3 / 4;
	int32_t bmp_height = bitmap.height_px();

	status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &source_image);
	status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &dest_image);
	status |= clSetKernelArg(kernel, 2, sizeof(int32_t), &bmp_width_reinterpreted);
	status |= clSetKernelArg(kernel, 3, sizeof(int32_t), &bmp_height);
	CHECK_STATUS("Failed to set kernel args.");

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clEnqueueNDRangeKernel");
#endif

	cl_event event;

#ifdef RETURN_ND_RANGE_TIME
	auto time_start = clock();
#endif

	status = clEnqueueNDRangeKernel(
		command_queue, kernel, 2, NULL, global_work_sizes, local_work_sizes, 0, NULL, &event);
	CHECK_STATUS("Failed to enqueue ND range kernel.");

	clWaitForEvents(1, &event);

#ifdef RETURN_ND_RANGE_TIME
	nd_range_time = clock() - time_start;
#endif

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clReleasexxx to end");
#endif
	
	clReleaseMemObject(source_image);
	clReleaseMemObject(dest_image);
	// On Intel UHD-620, if we only deallocate the two above, then in dubug mode
	// with a large bmp we will receive an error after some iterations due to memory over-alloc. 
	// But in release mode we receive no error. If we also deallocate the two below, there will be
	// a great performance hit for next iteration, possibly due to GPU cache misses.
	clReleaseMemObject(source_pixel_data_buffer);
	clReleaseMemObject(dest_pixel_data_buffer);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);

#ifdef USE_TIMER
	timer.StopAndPrint();
#endif
}



void BlurOpenCL::BlurOpenCL11ImageCopyHostPtr(
	const Bitmap& bitmap,
	Bitmap& bitmap_dest,
	const cl_device_id device_id,
	const std::string& binary_file_name,
	const std::string& function_name,
	const size_t* global_work_sizes,
	const size_t* local_work_sizes,
	double& nd_range_time)
{
#ifdef USE_TIMER
	CodeTimer timer;
	timer.Start("Begin to clSetKernelArg");
#endif
	cl_int status = CL_SUCCESS;

	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &status);
	CHECK_STATUS("Failed to create context.");

	cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &status);
	CHECK_STATUS("Failed to create command queue.");

	std::ifstream ifs(binary_file_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
	{
		std::cerr << BRACKETED_LINE("Failed to open binary program: " << binary_file_name);
		return;
	}

	ifs.seekg(0, std::ios::end);
	size_t binary_file_sizes[] = { ifs.tellg() };
	ifs.seekg(0, std::ios::beg);

	std::vector<unsigned char> program_binary(binary_file_sizes[0]);
	ifs.read((char*)program_binary.data(), binary_file_sizes[0]);

	ifs.close();

	const unsigned char* program_binaries[] = { program_binary.data() };

	cl_program program = clCreateProgramWithBinary(
		context, 1, &device_id, binary_file_sizes, program_binaries, NULL, &status);
	CHECK_STATUS("Failed to create program with binary.");

	status = clBuildProgram(program, 1, &device_id, "", NULL, NULL);
	CHECK_STATUS("Failed to build program.");

	cl_image_format image_format{
		CL_RGBA,
		CL_UNSIGNED_INT8
	};

	cl_mem source_image = clCreateImage2D(
		context,
		CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
		&image_format,
		bitmap.width_px() * 3 / 4,
		bitmap.height_px(),
		0,
		bitmap.pixel_data(),
		&status
	);
	CHECK_STATUS("Failed to create source bitmap image.");

	cl_mem dest_image = clCreateImage2D(
		context,
		CL_MEM_COPY_HOST_PTR | CL_MEM_WRITE_ONLY,
		&image_format,
		bitmap_dest.width_px() * 3 / 4,
		bitmap_dest.height_px(),
		0,
		bitmap_dest.pixel_data(),
		&status
	);
	CHECK_STATUS("Failed to create dest image.");

	cl_kernel kernel = clCreateKernel(program, function_name.data(), &status);
	CHECK_STATUS("Failed to create kernel.");

	int32_t bmp_width_reinterpreted = bitmap.width_px() * 3 / 4;
	int32_t bmp_height = bitmap.height_px();

	status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &source_image);
	status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &dest_image);
	status |= clSetKernelArg(kernel, 2, sizeof(int32_t), &bmp_width_reinterpreted);
	status |= clSetKernelArg(kernel, 3, sizeof(int32_t), &bmp_height);
	CHECK_STATUS("Failed to set kernel args.");

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clEnqueueNDRangeKernel");
#endif

	cl_event event;

#ifdef RETURN_ND_RANGE_TIME
	auto time_start = clock();
#endif

	status = clEnqueueNDRangeKernel(
		command_queue, kernel, 2, NULL, global_work_sizes, local_work_sizes, 0, NULL, &event);
	CHECK_STATUS("Failed to enqueue ND range kernel.");

	clWaitForEvents(1, &event);

#ifdef RETURN_ND_RANGE_TIME
	nd_range_time = clock() - time_start;
#endif

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clEnqueueReadImage to end");
#endif

	const size_t origin[] = { 0,0,0 };
	const size_t region[] = { bitmap_dest.width_px() * 3 / 4,bitmap.height_px(),1 };

	// this must be called even if the image is created with CL_MEM_USE_HOST_PTR
	status = clEnqueueReadImage(
		command_queue,
		dest_image, 
		CL_TRUE, 
		origin, 
		region, 
		0,
		0,
		bitmap_dest.pixel_data(),
		0, 
		NULL,
		NULL);
	CHECK_STATUS("Failed to enqueue read image.");

	clReleaseMemObject(source_image);
	clReleaseMemObject(dest_image);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);

#ifdef USE_TIMER
	timer.StopAndPrint();
#endif
}



void BlurOpenCL::BlurOpenCLCopyHostPtr(
	const Bitmap& bitmap,
	Bitmap& bitmap_dest,
	const cl_device_id device_id,
	const std::string& binary_file_name,
	const std::string& function_name,
	const size_t* global_work_sizes,
	const size_t* local_work_sizes,
	double& nd_range_time)
{
#ifdef USE_TIMER
	CodeTimer timer;
	timer.Start("Begin to clBuildProgram");
#endif
	cl_int status = CL_SUCCESS;

	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &status);
	CHECK_STATUS("Failed to create context.");

	cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &status);
	CHECK_STATUS("Failed to create command queue.");

	std::ifstream ifs(binary_file_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
	{
		std::cerr << BRACKETED_LINE("Failed to open binary program: " << binary_file_name);
		return;
	}

	ifs.seekg(0, std::ios::end);
	size_t binary_file_sizes[] = { ifs.tellg() };
	ifs.seekg(0, std::ios::beg);

	std::vector<unsigned char> program_binary(binary_file_sizes[0]);
	ifs.read((char*)program_binary.data(), binary_file_sizes[0]);

	ifs.close();

	const unsigned char* program_binaries[] = { program_binary.data() };

	cl_program program = clCreateProgramWithBinary(
		context, 1, &device_id, binary_file_sizes, program_binaries, NULL, &status);
	CHECK_STATUS("Failed to create program with binary.");

	status = clBuildProgram(program, 1, &device_id, "", NULL, NULL);
	CHECK_STATUS("Failed to build program.");

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clCreateBuffer");
#endif

	cl_mem source_pixel_data_buffer = clCreateBuffer(
		context,
		CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
		bitmap.pixel_data_alloc_size(),
		bitmap.pixel_data(),
		&status);
	CHECK_STATUS("Failed to create source bitmap pixel data buffer.");

	cl_mem dest_pixel_data_buffer = clCreateBuffer(
		context,
		CL_MEM_COPY_HOST_PTR | CL_MEM_WRITE_ONLY,
		bitmap_dest.pixel_data_alloc_size(),
		bitmap_dest.pixel_data(),
		&status);
	CHECK_STATUS("Failed to create dest bitmap pixel data buffer.");

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clCreateKernel to clSetKernelArg");
#endif

	cl_kernel kernel = clCreateKernel(program, function_name.data(), &status);
	CHECK_STATUS("Failed to create kernel.");

	int32_t bmp_width = bitmap.width_px();
	int32_t bmp_height = bitmap.height_px();

	status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &source_pixel_data_buffer);
	status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &dest_pixel_data_buffer);
	status |= clSetKernelArg(kernel, 2, sizeof(int32_t), &bmp_width);
	status |= clSetKernelArg(kernel, 3, sizeof(int32_t), &bmp_height);
	CHECK_STATUS("Failed to set kernel args.");

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clEnqueueNDRangeKernel");
#endif

	cl_event event;

#ifdef RETURN_ND_RANGE_TIME
	auto time_start = clock();
#endif

	status = clEnqueueNDRangeKernel(
		command_queue, kernel, 2, NULL, global_work_sizes, local_work_sizes, 0, NULL, &event);
	CHECK_STATUS("Failed to enqueue ND range kernel.");

	clWaitForEvents(1, &event);

#ifdef RETURN_ND_RANGE_TIME
	nd_range_time = clock() - time_start;
#endif

#ifdef USE_TIMER
	timer.StopAndPrint();
	timer.Start("clEnqueueReadBuffer to end");
#endif

	status = clEnqueueReadBuffer(
		command_queue,
		dest_pixel_data_buffer,
		CL_TRUE,
		0,
		bitmap_dest.pixel_data_alloc_size(),
		bitmap_dest.pixel_data(),
		0,
		NULL,
		NULL);
	CHECK_STATUS("Failed to read bitmap pixel data buffer.");

	clReleaseMemObject(source_pixel_data_buffer);
	clReleaseMemObject(dest_pixel_data_buffer);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);


#ifdef USE_TIMER
	timer.StopAndPrint();
#endif
}
