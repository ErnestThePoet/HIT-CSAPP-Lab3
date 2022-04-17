#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>

#include <immintrin.h>

#include "bitmap.h"

class BlurImpls
{
public:
	static void BlurNaive(Bitmap& bitmap);

	static void BlurMemoryBlocking(Bitmap& bitmap, const int block_size);

	static void BlurAVX(Bitmap& bitmap);

	static void BlurOpenMP(Bitmap& bitmap);
};

