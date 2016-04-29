#include "opencv2/opencv.hpp"  
#include "ViBe.h"  
#include <iostream>  
#include <cstdio>  
#include "MorphoFeatures.h"

using namespace cv;
using namespace std;
//RNG rng(12345);
void thresh_callback(Mat src_gray, int thresh, Mat frame);
int main(int argc, char* argv[])
{
	Mat frame, gray, mask, corners;
	static Mat lastFrame;
	VideoCapture capture;
	MorphoFeatures morpho;
	Mat element5(5, 5, CV_8U, Scalar(1));//�������еĽṹ��
	//RNG rng(12345);
	int i = 0;
	capture.open("..//vedio//test.avi");

	if (!capture.isOpened())
	{
		cout << "No camera or video input!\n" << endl;
		return -1;
	}

	ViBe_BGS Vibe_Bgs;
	int count = 0;

	while (1)
	{
		//i++;
		count++;
		capture >> frame;
		if (frame.empty())
			break;
		cvtColor(frame, gray, CV_RGB2GRAY);
		//imshow("input", frame);
		if (count<25)
		{
			if (count == 1)
			{
				Vibe_Bgs.init(gray);
				Vibe_Bgs.processFirstFrame(gray);
				cout << " Training GMM complete!" << endl;
			}
			else
			{
				Vibe_Bgs.testAndUpdate(gray);
				mask = Vibe_Bgs.getMask();
				morphologyEx(mask, mask, MORPH_OPEN, Mat());
				imshow("���һ֡��ֵ", mask);
			}
		}
		else
		{
			Vibe_Bgs.testAndUpdate(gray);
			mask = Vibe_Bgs.getMask();
			morphologyEx(mask, mask, MORPH_CLOSE, element5);//���б����㽫��������ͨ����
			lastFrame = mask;
			imshow("���һ֡", lastFrame);
			imshow("���һ֡��ֵ", frame);
			thresh_callback(lastFrame, 100, frame);
		}
		cout << count << endl;
		waitKey(10);
	}

	return 0;
}

/** @thresh_callback ���� */


void thresh_callback(Mat src_gray, int thresh, Mat frame)
{
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Rect afterBoundingRect;

	/// ʹ��Threshold����Ե
	//threshold(src_gray, threshold_output, thresh, 255, THRESH_BINARY);
	//imshow("����Ե", threshold_output);
	/// �ҵ�����
	findContours(src_gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// ����αƽ����� + ��ȡ���κ�Բ�α߽��
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	/*vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());*/

	for (int i = 0; i < contours.size(); i++)
	{
		//Mat a = Mat(contours_poly[i]);
		//approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		//boundingRect(contours[i]);
		afterBoundingRect = boundingRect(contours[i]);
		if(afterBoundingRect.area()>500)
			boundRect.push_back(boundingRect(contours[i]));
		//minEnclosingCircle(contours_poly[i], center[i], radius[i]);
	}


	/// ����������� + ��Χ�ľ��ο� + Բ�ο�
	Mat drawing = Mat::zeros(src_gray.size(), CV_8UC3);
	for (int i = 0; i< boundRect.size(); i++)
	{
		/*Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());*/
		rectangle(frame, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 255, 255), 2, 8, 0);
		//circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
	}

	/// ��ʾ��һ������
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", frame);
}