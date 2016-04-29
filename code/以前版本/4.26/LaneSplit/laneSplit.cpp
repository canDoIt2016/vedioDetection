#include "opencv2/opencv.hpp"  
#include <iostream>
#include "ViBe.h"  

using namespace cv;
using namespace std;

Mat org, dst, img, tmp;
Rect virtualLoop;
void on_mouse(int event, int x, int y, int flags, void *ustc);
int main()
{
	Mat frame, effectiveRange, gray, mask;
	VideoCapture capture;
	ViBe_BGS Vibe_Bgs;
	Mat element5(3, 3, CV_8U, Scalar(1));//闭运算中的结构子
	int count=0;
	capture.open("vedio//test.avi");
	if (!capture.isOpened())
	{
		cout << "No camera or video input!\n" << endl;
		return -1;
	}
	while (true)
	{
		++count;
		capture >> frame;
		frame.copyTo(org);
		if (org.empty())
		{
			cout << "空了" << endl;
			break;
		}
		
		if (count==1)
		{
			org.copyTo(img);
			org.copyTo(tmp);
			imshow("img", img);
			//设定检测的区域
			setMouseCallback("img", on_mouse, 0);//调用回调函数,画矩形
			waitKey(0);
			//namedWindow("dst");
			imshow("dst", dst);
			setMouseCallback("img", on_mouse, 0);//调用回调函数,画矩形
			
		}
		else
		{
			if (count==2)
			{
				cvtColor(frame(virtualLoop), gray, CV_RGB2GRAY);//将帧数图像灰度化
				//vibe初始化背景图像
				Vibe_Bgs.init(gray);
				Vibe_Bgs.processFirstFrame(gray);
				cout << " Training GMM complete!" << endl;
			}
			else
			{
				cvtColor(frame(virtualLoop), gray, CV_RGB2GRAY);//将帧数图像灰度化
				Vibe_Bgs.testAndUpdate(gray);
				mask = Vibe_Bgs.getMask();
				morphologyEx(mask, mask, MORPH_CLOSE, element5);//进行闭运算将点区域连通起来
				cout << endl << "x:" << virtualLoop.x << "y:" << virtualLoop.y<<"with:"<<virtualLoop.width<<"height:"<<virtualLoop.height<<endl;
				imshow("img", mask);
			}
			
		}
		
		waitKey(10);
	}
	return 0;
}

void on_mouse(int event, int x, int y, int flags, void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号  
{
	static Point pre_pt = (-1, -1);//初始坐标  
	static Point cur_pt = (-1, -1);//实时坐标  
	char temp[16];
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处划圆  
	{
		org.copyTo(img);//将原始图片复制到img中  
		sprintf(temp, "(%d,%d)", x, y);
		pre_pt = Point(x, y);
		putText(img, temp, pre_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255), 1, 8);//在图像窗口上显示坐标  
		circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//划圆  
		imshow("img", img);
	}
	else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//左键没有按下的情况下鼠标移动的处理函数  
	{
		img.copyTo(tmp);//将img复制到临时图像tmp上，用于显示实时坐标  
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));//只是实时显示鼠标移动的坐标  
		imshow("img", tmp);
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划矩形  
	{
		img.copyTo(tmp);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
		line(tmp, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//在临时图像上实时显示鼠标拖动时形成的矩形  
		imshow("img", tmp);
	}
	else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图像上划矩形  
	{
		org.copyTo(img);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(img, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
		circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
		line(img, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点，将矩形画到img上  
		imshow("img", img);
		img.copyTo(tmp);
		//截取矩形包围的图像，并保存到dst中  
		int width = abs(pre_pt.x - cur_pt.x);
		int height = abs(pre_pt.y - cur_pt.y);
		/*if (width == 0 || height == 0)
		{
			printf("width == 0 || height == 0");
			return;
		}*/
		dst = org(Rect(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height));
		virtualLoop = Rect(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height);
		
		/*waitKey(10);*/
	}
}