#pragma once
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "bitmap.h"
#include "utilities.h"

class BmpHelper
{
public:
	static Bitmap Load(const std::string& file_name);

	static void PrintBitmapInfo(const Bitmap& bitmap);
	
	static void Save(const Bitmap& bitmap, const std::string& file_name);
};

