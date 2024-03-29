#pragma once

////////////////////////////// Options /////////////////////////////////

#define TAISHAN // means ARM + Linux + No OpenCL + No Warmup

#define OPENCL

//#define USE_WARMUP
#define TEST_NAIVE
#define TEST_LOOP_UNROLL
#define TEST_CACHE_OPT
#define TEST_SIMD
#define TEST_OPENMP
#define TEST_SIMD_OPENMP
#define TEST_OPENCL

//#define SAVE_NAIVE
//#define SAVE_OPTIMIZATIONS
//#define SAVE_OPENCL

// OpenCL Blur Settings
//#define USE_TIMER
#define RETURN_ND_RANGE_TIME

////////////////////////////////////////////////////////////////////////

#ifdef TAISHAN
#undef OPENCL
#undef USE_WARMUP
#undef TEST_OPENCL
#endif