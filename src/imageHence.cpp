#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include "imageHence.h"

#include "imageCommon.h"

using namespace cv;
using namespace std;

static void GaussBlurProc(vector<unsigned char> inBuffer, vector<unsigned char> &outBuffer) {
    Size imgSize(3,5);

    GaussianBlur(inBuffer, outBuffer, imgSize, 3, 0, 4);

    Mat GsBlrImg(H, W, CV_8UC1, &outBuffer[0]);

    namedWindow("Display GaussProc outImage", WINDOW_AUTOSIZE);
    imshow("Display GaussProc outImage", GsBlrImg);
    // waitKey(0);
}

static void GaborProc(vector<unsigned char> inBuffer, vector<unsigned char> &outBuffer) {

}

void doHence(vector<vector<unsigned char> > rawData, vector<unsigned char> &outputBuffer) {
    //构造Mat Iamge
    Mat inImage(H, W, CV_8UC1, &rawData[0][0]);

    //显示输入原始数据
    namedWindow("Display Orignal inImage", WINDOW_AUTOSIZE);
    imshow("Display Orignal inImage", inImage);

    //创建一个方向是PI/4而尺度是3的gabor  
    // double Sigma = 2*PI;    
    // double F = sqrt(2.0);    
    // CvGabor *gabor1 = new CvGabor;    
    // gabor1->Init(PI/4, 3, Sigma, F); 

    // //获取载入图像的gabor滤波响应的实部并且显示  
    // IplImage *reimg = cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 1);  
    // gabor1->conv_img(image, reimg, CV_GABOR_REAL);

    //进行高斯模糊处理
    GaussBlurProc(rawData[0], outputBuffer);

}
