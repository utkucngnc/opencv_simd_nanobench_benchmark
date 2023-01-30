#include <iostream>
#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#define SIMD_OPENCV_ENABLE
#include <Simd/SimdLib.hpp>

using namespace cv;
using std::cout;

int const max_val = 255; //Maximum pixel value
int pcent = 45; //Percentage for binary thresholding
int thold = max_val * pcent / 100; //Threshold for binary thresholding

int main(int argc, char **argv)
{   
    if (argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    setNumThreads(0); //Prevent multi - threading for OpenCV
    SimdSetThreadNumber(1); //Prevent multi - threading for SIMD
    
    cout << "Image is read: " << argv[1] << std::endl;

    Mat image = imread(argv[1], IMREAD_GRAYSCALE); //Initialize OpenCV images
    Mat binarized_im, simd_bin_mat, bin_im_new;

    typedef Simd::View<Simd::Allocator> View; //Initialize SIMD template
    
    View simd_view, simd_bin; //Initialize SIMD images
    simd_view.Load(argv[1],View::Gray8);
    simd_bin = simd_view;

    ankerl::nanobench::Bench().minEpochIterations(5000).run("opencv_threshold", [&] {
            threshold(image, binarized_im, thold, max_val, 0); //OpenCV threshold function
    }); //OpenCV Binary Thresholding Benchmark

    ankerl::nanobench::Bench().minEpochIterations(5000).run("opencv_adaptive_mean_threshold", [&] {
            adaptiveThreshold(binarized_im, bin_im_new, max_val, ADAPTIVE_THRESH_MEAN_C,
                THRESH_BINARY, 7, 2); //OpenCV mean adaptive threshold function
    }); //OpenCV Mean Adaptive Thresholding Benchmark

    ankerl::nanobench::Bench().minEpochIterations(5000).run("opencv_adaptive_gaussian_threshold", [&] {
            adaptiveThreshold(binarized_im, bin_im_new, max_val, ADAPTIVE_THRESH_GAUSSIAN_C,
                THRESH_BINARY, 7, 2); //OpenCV gaussian adaptive threshold function
    }); //OpenCV Gaussian Adaptive Thresholding Benchmark

    ankerl::nanobench::Bench().minEpochIterations(5000).run("simd_threshold", [&] {
        SimdBinarization(
                simd_view.data, simd_view.stride, simd_view.width,
                simd_view.height, thold, max_val, 0, simd_bin.data, 
                simd_view.stride, SimdCompareType::SimdCompareGreaterOrEqual
                ); //SIMD Threshold Function
    }); //SIMD Binary Thresholding Benchmark

    ankerl::nanobench::Bench().minEpochIterations(5000).run("simd_adaptive_threshold", [&] {
        SimdAveragingBinarizationV2(
                simd_view.data, simd_view.stride, simd_view.width,
                simd_view.height, 7, 2, max_val, 0, simd_bin.data, 
                simd_view.stride); //SIMD Threshold Function
    }); //SIMD Adaptive Thresholding Benchmark

    
    
    if (!image.data || !simd_view.data) {
        printf("No image data \n");
        return -1;
    } //Check Image Data

    /*
    //Uncomment to show image 
    
    simd_bin_mat = simd_bin;
    
    namedWindow("Display Image", WINDOW_NORMAL);
    imshow("Display Image", simd_bin_mat);
    waitKey(0);
    
    */
    return 0;
}