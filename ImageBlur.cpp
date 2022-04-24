#include <iostream>
#include <string>

#include "macros.h"

#include "bmp_helper.h"
#include "code_timer.h"

#include "blur_impls.h"

#ifdef OPENCL
#include "blur_opencl.h"
#include "cl_device_helper.h"
#endif



#define UHD620_PROGRAM_BINARY_NAME "./cl_kernels/uhd620_cl2.0_i.bin"
#define GFX803_PROGRAM_BINARY_NAME "./cl_kernels/gfx803_cl2.0_a.bin"


int main()
{
    CodeTimer timer;

    std::cout << "Enter a bmp file name to run benchmark on: ";
    std::string bmp_file_name;
    std::cin >> bmp_file_name;

    std::cout << "Enter benchmark iteration count: ";
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


#if defined(TEST_SIMD)&&!defined(TAISHAN)
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

#if defined(TEST_SIMD)&&defined(TAISHAN)
    bitmap_copy = original_bitmap;
    timer.Start("Arm Neon");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurNeon(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/neon.bmp");
#endif
#endif


#ifdef TEST_OPENMP
    bitmap_copy = original_bitmap;
    timer.Start("OpenMP Multithreading");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurOpenMP(original_bitmap, bitmap_copy);
    }
    timer.StopAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/openmp.bmp");
#endif
#endif


#if defined(TEST_SIMD_OPENMP)&&defined(TAISHAN)
    bitmap_copy = original_bitmap;
    timer.Start("Arm Neon + OpenMP");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurNeonOpenMP(original_bitmap, bitmap_copy);
    }
    timer.StopAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/neon_openmp.bmp");
#endif
#endif


#if defined(TEST_SIMD_OPENMP)&&!defined(TAISHAN)
    bitmap_copy = original_bitmap;
    timer.Start("AVX + OpenMP");
    for (int i = 0; i < test_count; i++)
    {
        BlurImpls::BlurAVXOpenMP(original_bitmap, bitmap_copy);
    }
    timer.StopAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/avx_openmp.bmp");
#endif
#endif


#if defined(OPENCL)&&defined(TEST_OPENCL)
    bitmap_copy = original_bitmap;

    CLDeviceHelper helper;
    helper.Initialize();

    std::cout << std::string(100, '*') << std::endl;

    //helper.PrintAllDevices();
    //cl_device_id device_id = helper.GetDeviceIdFromInput();

    cl_device_id uhd620_device_id = helper.GetDeviceIdWithPreference(CLDeviceHelper::kPreferenceIAN);
    cl_device_id gfx803_device_id = helper.GetDeviceIdWithPreference(CLDeviceHelper::kPreferenceANI);

    //std::cout << std::string(100, '*') << std::endl;

    size_t work_item_sizes[] = { bitmap_copy.width_px(),bitmap_copy.height_px() };
    size_t global_work_sizes[2]{};
    size_t local_work_sizes[2]{};

    helper.GetSuitableGlobalLocalSize(
        uhd620_device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);

    double nd_range_time = 0.0;
    double total_nd_range_time = 0.0;

    timer.Start("OpenCL - Intel UHD620 Zero-Copy");
    for (int i = 0; i < test_count; i++)
    {
        BlurOpenCL::BlurOpenCLZeroCopy(
            original_bitmap,
            bitmap_copy, 
            uhd620_device_id,
            UHD620_PROGRAM_BINARY_NAME,
            "BlurNaive", 
            global_work_sizes, 
            local_work_sizes,
            nd_range_time);

        total_nd_range_time += nd_range_time;
    }
    timer.StopAndPrint(naive_time);
    timer.PrintElapse(total_nd_range_time, "clEnqueueNDRangeKernel", naive_time);
    std::cout << std::endl;



    bitmap_copy = original_bitmap;
    total_nd_range_time = 0.0;

    helper.GetSuitableGlobalLocalSize(
        gfx803_device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);

    //timer.Start("OpenCL - AMD RX550X CopyHostPtr");
    for (int i = 0; i < test_count; i++)
    {
        BlurOpenCL::BlurOpenCLCopyHostPtr(
            original_bitmap,
            bitmap_copy,
            gfx803_device_id,
            GFX803_PROGRAM_BINARY_NAME,
            "BlurNaive",
            global_work_sizes,
            local_work_sizes,
            nd_range_time);

        total_nd_range_time += nd_range_time;
    }
    //timer.StopAndPrint(naive_time);
    timer.PrintElapse(total_nd_range_time, "OpenCL - AMD RX550X CopyHostPtr", naive_time);
    std::cout << std::endl;



    if (bitmap_copy.width_px() % 4 == 0)
    {
        bitmap_copy = original_bitmap;
        total_nd_range_time = 0.0;

        work_item_sizes[0]=bitmap_copy.width_px() * 3 / 4;
        work_item_sizes[1] = bitmap_copy.height_px();

        helper.GetSuitableGlobalLocalSize(
            uhd620_device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);

        timer.Start("OpenCL - Intel UHD620 Zero-Copy using Image");
        for (int i = 0; i < test_count; i++)
        {
            BlurOpenCL::BlurOpenCLImageZeroCopy(
                original_bitmap,
                bitmap_copy,
                uhd620_device_id,
                UHD620_PROGRAM_BINARY_NAME,
                "BlurImage",
                global_work_sizes,
                local_work_sizes,
                nd_range_time);

            total_nd_range_time += nd_range_time;
        }
        timer.StopAndPrint(naive_time);
        timer.PrintElapse(total_nd_range_time, "clEnqueueNDRangeKernel", naive_time);
        std::cout << std::endl;


        
        bitmap_copy = original_bitmap;
        total_nd_range_time = 0.0;

        helper.GetSuitableGlobalLocalSize(
            gfx803_device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);


        //timer.Start("OpenCL - AMD RX550X CopyHostPtr using Image");
        for (int i = 0; i < test_count; i++)
        {
            BlurOpenCL::BlurOpenCL11ImageCopyHostPtr(
                original_bitmap,
                bitmap_copy,
                gfx803_device_id,
                GFX803_PROGRAM_BINARY_NAME,
                "BlurImage",
                global_work_sizes,
                local_work_sizes,
                nd_range_time);

            total_nd_range_time += nd_range_time;
        }
        //timer.StopAndPrint(naive_time);
        timer.PrintElapse(total_nd_range_time, "OpenCL - AMD RX550X CopyHostPtr using Image", naive_time);
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Ensure bmp has a width that is a multiple of 4 to use OpenCL Image." << std::endl;
    }
#ifdef SAVE_OPENCL
    BmpHelper::Save(bitmap_copy, "./blurred/opencl.bmp");
#endif
#endif

    return 0;
}
