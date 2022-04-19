#include "bmp_helper.h"
#include "code_timer.h"

#include "blur_impls.h"
#include "blur_opencl.h"
#include "cl_device_helper.h"


constexpr int kTestCount = 1;

#define USE_WARMUP
#define TEST_NAIVE
#define TEST_LOOP_UNROLL
#define TEST_CACHE_OPT
#define TEST_AVX
#define TEST_OPENMP
#define TEST_OPENCL

//#define COMPILE_OPENCL_PROGRAM_ONLY
#define CL_PROGRAM_BINARY_NAME "./cl_kernels/uhd620_cl2.0_i.bin"

//#define SAVE_NAIVE
//#define SAVE_OPTIMIZATIONS
#define SAVE_OPENCL


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
    Bitmap original_bitmap = BmpHelper::Load("./bmpsrc/scenery.bmp");

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
        << kTestCount
        << " time(s) per candidate."
        << std::endl;

    Bitmap bitmap_copy{};


#ifdef USE_WARMUP
    std::cout << "-------------------- Turbo Boost Warming Up --------------------" << std::endl;

    bitmap_copy = original_bitmap;
    timer.Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();

    timer.Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();

    timer.Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();

    timer.Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();

    timer.Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    timer.StopAndPrint();
#endif


    std::cout << "-------------------- Benchmark Starts Here --------------------" << std::endl;


#ifdef TEST_NAIVE
    bitmap_copy = original_bitmap;
    timer.Start("Naive Implementation");
    for (int i = 0; i < kTestCount; i++)
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
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll3(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 5x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll5(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 7x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll7(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 9x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll9(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 11x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll11(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 13x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll13(bitmap_copy);
    }
    timer.StopAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    timer.Start("Loop Unroll 15x");
    for (int i = 0; i < kTestCount; i++)
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
    for (int i = 0; i < kTestCount; i++)
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
    for (int i = 0; i < kTestCount; i++)
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
    for (int i = 0; i < kTestCount; i++)
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

    //std::cout << "***************************************************************" << std::endl;

    //helper.PrintAllDevices();
    //cl_device_id device_id = helper.GetDeviceIdFromInput();

    cl_device_id device_id = helper.GetDeviceIdWithPreference(CLDeviceHelper::kPreferenceIAN);

    //std::cout << "***************************************************************" << std::endl;

    size_t work_item_sizes[] = { bitmap_copy.width_px(),bitmap_copy.height_px() };
    size_t global_work_sizes[2]{};
    size_t local_work_sizes[2]{};

    helper.GetSuitableGlobalLocalSize(
        device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);

    timer.Start("OpenCL on GPU - Naive");
    for (int i = 0; i < kTestCount; i++)
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
        for (int i = 0; i < kTestCount; i++)
        {
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



        bitmap_copy = original_bitmap;
        // target work items on dim0: reinterpreted-width except 2 border pixels: 1-3:0; 4-7:1; 8-11:2;...
        // then reinterpreted-width: 3-5:0; 6-9:1; 10-13:2;...
        work_item_sizes[0] = (bitmap_copy.width_px() * 3 / 4 - 2) / 4;
        work_item_sizes[1] = bitmap_copy.height_px();
        helper.GetSuitableGlobalLocalSize(
            device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);

        timer.Start("OpenCL on GPU - Image, WPI=4");
        for (int i = 0; i < kTestCount; i++)
        {
            BlurOpenCLImageZeroCopy(
                original_bitmap,
                bitmap_copy,
                device_id,
                CL_PROGRAM_BINARY_NAME,
                "BlurImageWPI4",
                global_work_sizes,
                local_work_sizes);
        }
        timer.StopAndPrint(naive_time);



        bitmap_copy = original_bitmap;
        // target work items on dim0: reinterpreted-width except 2 border pixels: 1-7:0; 8-15:1; 16-23:2;...
        // then reinterpreted-width: 3-9:0; 10-17:1; 18-25:2;...
        work_item_sizes[0] = (bitmap_copy.width_px() * 3 / 4 - 2) / 8;
        work_item_sizes[1] = bitmap_copy.height_px();
        helper.GetSuitableGlobalLocalSize(
            device_id, 2, work_item_sizes, global_work_sizes, local_work_sizes);

        timer.Start("OpenCL on GPU - Image, WPI=8");
        for (int i = 0; i < kTestCount; i++)
        {
            BlurOpenCLImageZeroCopy(
                original_bitmap,
                bitmap_copy,
                device_id,
                CL_PROGRAM_BINARY_NAME,
                "BlurImageWPI8",
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
