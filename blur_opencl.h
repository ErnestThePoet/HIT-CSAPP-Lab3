#pragma once
#include <iostream>
#include <fstream>
#include <vector>

#include "bitmap.h"
#include "cl_device_helper.h"
#include "CL/cl.h"

void BlurOpenCL(Bitmap& bitmap);