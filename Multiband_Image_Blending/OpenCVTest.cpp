//// OpenCVTest.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
////
#include<iostream>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include<vector>

using namespace std;
using namespace cv;

vector<cv::Mat> GaussianPyramid(cv::Mat img, int depth=5) {
    vector<cv::Mat> pyramid;
    cv::Mat src = img;
    
    for (int i = 0; i < depth; i++) {
        pyramid.push_back(src);
        cv::pyrDown(src, src);
    }
    return pyramid;
}

vector<cv::Mat> LaplacianPyramid(cv::Mat img, int depth = 5) {
    vector<cv::Mat> pyramid;
    cv::Mat src = img;

    for (int i = 0; i < depth-1; i++) {
        cv::Mat small, tmp;
        cv::pyrDown(src, small);
        cv::pyrUp(small, tmp, src.size());

        pyramid.push_back(src-tmp);
        src = small;
    }
    pyramid.push_back(src);
    return pyramid;
}

cv::Mat reconstruct(const vector<cv::Mat>& pyramid) {
    cv::Mat ret = pyramid.back();
    for (int i = pyramid.size() - 2; i>0 ;i-- ) {
        cv::pyrUp(ret, ret, pyramid[i].size());
        ret += pyramid[i];
    }
    return ret;
}

Mat blend(Mat img1, Mat img2, Mat mask) {
    Mat mul1, mul2, mask2, sum;
    multiply(img1, mask, mul1);
    mask2 = Scalar::all(1) - mask;
    multiply(img2, mask2, mul2);

    add(mul1, mul2, sum);
    return sum;
}

int main()
{
    int depth = 5;
    cv::Mat image1 = cv::imread("C:\\Users\\ekdls\\Desktop\\apple.jpg");
    cv::Mat image2 = cv::imread("C:\\Users\\ekdls\\Desktop\\orange.jpg");
    cv::Mat mask = cv::imread("C:\\Users\\ekdls\\Desktop\\mask.png");

    image1.convertTo(image1, CV_32F, 1 / 255.f);
    image2.convertTo(image2, CV_32F, 1 / 255.f);
    mask.convertTo(mask, CV_32F, 1 / 255.f);
  
    // apple image에 대한 Laplacian pyramid, orange image에 대한 Laplacian pyramid를 만들고,
    vector<cv::Mat> applePyr = LaplacianPyramid(image1, depth);
    vector<cv::Mat> orangePyr = LaplacianPyramid(image2, depth);
    //각각의 band를 합성하기 위한 mask 영상은 원본 mask 영상의 Gaussian pyramid를 생성하여 사용합니다.
    vector<cv::Mat> maskPyr = GaussianPyramid(mask, depth);
    vector<cv::Mat> result;
    //result(i) = mask(i)*apple(i) + (1-mask(i))*orange(i) 와 같이 결과 영상의 band를 계산합니다.
    for (int i = 0; i < depth; i++ ) {
        Mat tmp;
        tmp = blend(applePyr[i], orangePyr[i], maskPyr[i]);
        result.push_back(tmp);
    }
    auto res = reconstruct(result);

    imshow("res", res);
    waitKey();
 
    return 0;


}
//
////각각 라플라시안 피라미드를 만들고
////mask이미지 하얀색 사과 까만색 귤 의 가우시안 피라미드
////
////reconstruction

