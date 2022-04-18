#include "blur_opencl.h"

void BlurOpenCL(Bitmap& bitmap)
{
	CLDeviceHelper helper;
	helper.Initialize();
	helper.PrintAllDevices();

	cl_device_id device_id = helper.GetDeviceIdFromInput();



}
