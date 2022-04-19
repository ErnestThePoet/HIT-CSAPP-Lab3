#include <iostream>
#include <string>

#include "bmp_helper.h"
#include "code_timer.h"

#include "blur_impls.h"
#include "blur_opencl.h"
#include "cl_device_helper.h"


//#define COMPILE_OPENCL_PROGRAM_ONLY

#define USE_WARMUP
#define TEST_NAIVE
#define TEST_LOOP_UNROLL
#define TEST_CACHE_OPT
#define TEST_AVX
#define TEST_OPENMP
#define TEST_OPENCL

// steps to change device: 1.change binary name; 2.change preference list; 3.change image function
#define CL_PROGRAM_BINARY_NAME "./cl_kernels/uhd620_cl2.0_i.bin"

//#define SAVE_NAIVE
//#define SAVE_OPTIMIZATIONS
//#define SAVE_OPENCL


int main()
{
    CLDeviceHelper helper;

#ifdef COMPILE_OPENCL_PROGRAM_ONLY
    helper.Initialize();
    helper.PrintAllDevices();
    cl_device_id compile_target_device_id = helper.GetDeviceIdFromInput();

    CompileOpenCLKernelBinary(compile_target_device_id,"-cl-std=CL2.0");
    return 0;
#endif

    CodeTimer timer;

    std::cout << "Enter a bmp file name to run benchmark on: ";
    std::string bmp_file_name;
    std::cin >> bmp_file_name;

    std::cout << "Enter the benchmark iteration count: ";
    int test_count = 1;
    std::cin >> test_count;

    Bitmap original_bitmap = BmpHelper::Load(bmp_file_name);

    if (original_bitmap.info_header.bits_per_pixel != 24)
    {
        std::cout << BRACKETED_LINE(
            "Cannot start benchmark: You must load a 24-bit bitmap to run the benchmark.");
        return 1;
    }

    //BmpHelper::PrintBitmapInfo(original_bitmap);

    std::cout
        << "Today's benchmark will be run on a 24-bit "
        << original_bitmap.width_px()
        << " * "
        << original_bitmap.height_px()
        << " bitmap "
        << test_count
        << " time(s) per candidate."
        << std::endl;

    Bitmap bitmap_copy{};


#ifdef USE_WARMUP
    std::cout
        << std::string(38, '-')
        << " Turbo Boost Warming Up "
        << std::string(38, '-')
        << std::endl;

    bitmap_copy = original_bitmap;
    timer.Start("Naive Implementation WARMUP");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();

    timer.Start("Naive Implementation WARMUP");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();

    timer.Start("Naive Implementation WARMUP");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();

    timer.Start("Naive Implementation WARMUP");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();

    timer.Start("Naive Implementation WARMUP");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();
#endif


    std::cout 
        << std::string(38, '-')
        << " Benchmark Starts Here " 
        << std::string(38, '-')
        << std::endl;


#ifdef TEST_NAIVE
    bitmap_copy = original_bitmap;
    timer.Start("Naive Implementation");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    double naive_time = timer.StopAndPrint();

#ifdef SAVE_NAIVE
    BmpHelper::Save(bitmap_copy, "./blurred/naive_impl.bmp");
#endif
#endif


#ifdef TEST_LOOP_UNROLL
    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 3x");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurLoopUnroll3(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 5x");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurLoopUnroll5(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 7x");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurLoopUnroll7(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 9x");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurLoopUnroll9(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 11x");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurLoopUnroll11(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 13x");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurLoopUnroll13(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 15x");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurLoopUnroll15(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/loop_unroll.bmp");
#endif
#endif


#ifdef TEST_CACHE_OPT
    bitmap_copy = original_bitmap;
    timer.Start(std::string("Cache Optimization"));
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurCacheOpt(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/cache_opt.bmp");
#endif
#endif


#ifdef TEST_AVX
    bitmap_copy = original_bitmap;
    timer.Start("AVX");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurAVX(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/avx.bmp");
#endif
#endif


#ifdef TEST_OPENMP
    bitmap_copy = original_bitmap;
    timer.Start("AVX + OpenMP Multithreading");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurAVXOpenMP(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/openmp.bmp");
#endif
#endif


#ifdef TEST_OPENCL
    bitmap_copy = original_bitmap;

    helper.Initialize();

    //std::cout << std::string(100, '*') << std::endl;

    //helper.PrintAllDevices();
    //cl_device_id device_id = helper.GetDeviceIdFromInput();

    cl_device_id device_id = helper.GetDeviceIdWithPreference(CLDeviceHelper::kPreferenceIAN);

    //std::cout << std::string(100, '*') << std::endl;

    size_t work_item_sizes[] = { bitmap_copy.width_px(),bitmap_copy.height_px() };
    size_t global_work_sizes[2]{};
    size_t local_work_sizes[2]{};

    helper.GetSuitableGlobalLocalSize(
        device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);

    timer.Start("OpenCL on GPU - Naive");
    for (int i = 0; i < test_count; i++)
    {
        BlurOpenCLZeroCopy(
            original_bitmap,
            bitmap_copy, 
            device_id, 
            CL_PROGRAM_BINARY_NAME,
            "BlurNaive", 
            global_work_sizes, 
            local_work_sizes);
    }
    timer.StopAndPrint(naive_time);


    if (bitmap_copy.width_px() % 4 == 0)
    {
        bitmap_copy = original_bitmap;

        work_item_sizes[0]=bitmap_copy.width_px() * 3 / 4;
        work_item_sizes[1] = bitmap_copy.height_px();

        helper.GetSuitableGlobalLocalSize(
            device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);

        timer.Start("OpenCL on GPU - Image");
        for (int i = 0; i < test_count; i++)
        {
            // must use BlurOpenCL11ImageZeroCopy for gfx803 to avoid crashes.
            BlurOpenCLImageZeroCopy(
                original_bitmap,
                bitmap_copy,
                device_id,
                CL_PROGRAM_BINARY_NAME,
                "BlurImage",
                global_work_sizes,
                local_work_sizes);
        }
        timer.StopAndPrint(naive_time);
    }
    else
    {
        std::cout << "Ensure bmp has a width that is a multiple of 4 to use OpenCL Image." << std::endl;
    }
#ifdef SAVE_OPENCL
    BmpHelper::Save(bitmap_copy, "./blurred/opencl.bmp");
#endif
#endif

    //getchar();

    return 0;
}
