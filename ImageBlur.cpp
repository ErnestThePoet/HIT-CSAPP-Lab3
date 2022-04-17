#include "bmp_helper.h"
#include "code_timer.h"

#include "blur_impls.h"
#include "blur_opencl.h"


constexpr int kTestCount = 1;

#define USE_WARMUP
#define TEST_NAIVE
#define TEST_LOOP_UNROLL
#define TEST_CACHE_OPT
//#define TEST_AVX
//#define TEST_OPENMP
//#define TEST_OPENCL


//#define SAVE_NAIVE
//#define SAVE_OPTIMIZATIONS

int main()
{
    Bitmap original_bitmap = BmpHelper::Load("./bmpsrc/lines.bmp");

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
    CodeTimer::Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    CodeTimer::EndAndPrint();

    CodeTimer::Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    CodeTimer::EndAndPrint();

    CodeTimer::Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    CodeTimer::EndAndPrint();

    CodeTimer::Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    CodeTimer::EndAndPrint();

    CodeTimer::Start("Naive Implementation ------- WARMUP ONLY");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    CodeTimer::EndAndPrint();
#endif


    std::cout << "-------------------- Benchmark Starts Here --------------------" << std::endl;


#ifdef TEST_NAIVE
    bitmap_copy = original_bitmap;
    CodeTimer::Start("Naive Implementation");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurNaive(bitmap_copy);
    }
    double naive_time = CodeTimer::EndAndPrint();

#ifdef PRINT_NAIVE
    BmpHelper::Save(bitmap_copy, "./blurred/naive_impl.bmp");
#endif
#endif


#ifdef TEST_LOOP_UNROLL
    bitmap_copy = original_bitmap;
    CodeTimer::Start("Loop Unroll 3x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll3(bitmap_copy);
    }
    CodeTimer::EndAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    CodeTimer::Start("Loop Unroll 5x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll5(bitmap_copy);
    }
    CodeTimer::EndAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    CodeTimer::Start("Loop Unroll 7x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll7(bitmap_copy);
    }
    CodeTimer::EndAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    CodeTimer::Start("Loop Unroll 9x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll9(bitmap_copy);
    }
    CodeTimer::EndAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    CodeTimer::Start("Loop Unroll 11x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll11(bitmap_copy);
    }
    CodeTimer::EndAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    CodeTimer::Start("Loop Unroll 13x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll13(bitmap_copy);
    }
    CodeTimer::EndAndPrint(naive_time);

    bitmap_copy = original_bitmap;
    CodeTimer::Start("Loop Unroll 15x");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurLoopUnroll15(bitmap_copy);
    }
    CodeTimer::EndAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/loop_unroll.bmp");
#endif
#endif


#ifdef TEST_CACHE_OPT
    bitmap_copy = original_bitmap;
    CodeTimer::Start(std::string("Cache Optimization"));
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurCacheOpt(bitmap_copy);
    }
    CodeTimer::EndAndPrint(naive_time);
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/cache_opt.bmp");
#endif
#endif


#ifdef TEST_AVX
    bitmap_copy = original_bitmap;
    CodeTimer::Start("AVX");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurAVX(bitmap_copy);
    }
    CodeTimer::EndAndPrint();
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/avx.bmp");
#endif
#endif


#ifdef TEST_OPENCL
    bitmap_copy = original_bitmap;
    CodeTimer::Start("OpenCL on GPU");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurOpenCL(bitmap_copy);
    }
    CodeTimer::EndAndPrint();
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/opencl.bmp");
#endif
#endif


#ifdef TEST_OPENMP
    bitmap_copy = original_bitmap;
    CodeTimer::Start("OpenMP Multithreading");
    for (int i = 0; i < kTestCount; i++)
    {
        BlurImpls::BlurOpenMP(bitmap_copy);
    }
    CodeTimer::EndAndPrint();
#ifdef SAVE_OPTIMIZATIONS
    BmpHelper::Save(bitmap_copy, "./blurred/openmp.bmp");
#endif
#endif


    //BmpHelper::Save(bitmap_copy, "./blurred/naive_impl.bmp");

    //getchar();

    return 0;
}
