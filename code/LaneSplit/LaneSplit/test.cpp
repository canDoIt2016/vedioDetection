#include <opencv2/opencv.hpp>  
#include <iostream> 

using namespace std;
using namespace cv;

int main(){
	vector<Vec4i> lines;//限定区域中检出的白线存放的位置
	Mat edge, color_edge,black_image;
	Mat img = imread("image//test2.jpg");
	Canny(img, edge, 50, 200, 3);
	HoughLinesP(edge, lines, 1, CV_PI / 180, 80, 30, 10);
	cvtColor(edge, color_edge, CV_GRAY2BGR);
	//画出边界线
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(black_image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 2);
	}
	imshow("image", black_image);
	waitKey(0);
	return 0;
}