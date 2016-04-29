#include "opencv2/opencv.hpp"  
#include <iostream>
#include "ViBe.h"  

using namespace cv;
using namespace std;

Mat org, dst, img, tmp;
Mat imgL, dstL, tmpL;
bool drawLine = false;
Rect virtualLoop;
vector<Vec4i> lines;//限定区域中检出的白线存放的位置
int laneNum;//要划定的车道数
void on_mouse(int event, int x, int y, int flags, void *ustc);
void drawLineFunc();
void drawDefault(int x, int y, string windowName, Mat windowImage, Mat preImage, Point pre_pt, Point cur_pt, int flag);
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
			imshow("dst", dst);
			cout << "请输入准备划定的直线数：" ;
			cin >> laneNum;
			drawLine = true;
			dst.copyTo(imgL);
			dst.copyTo(tmpL);
			/*for (int i = 0; i < laneNum; i++)
			{*/
				setMouseCallback("dst", on_mouse, 0);//调用回调函数,画直线
			//}
			waitKey(0);
			
			//imshow("dst", dstL);
		}
		else
		{
			lines;
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
	switch (drawLine)
	{
	case false:
		if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处划圆  
			{
				pre_pt = Point(x, y);
				drawDefault(x, y, "img", img, org, pre_pt, cur_pt, 0);
			}
			else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划矩形  
			{
				cur_pt = Point(x, y);
				drawDefault(x, y, "img", img, tmp, pre_pt, cur_pt, 1);
			}
			else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图像上划矩形  
			{
				cur_pt = Point(x, y);
				drawDefault(x, y, "img", img, org, pre_pt, cur_pt, 1);
				img.copyTo(tmp);
				//截取矩形包围的图像，并保存到dst中  
				int width = abs(pre_pt.x - cur_pt.x);
				int height = abs(pre_pt.y - cur_pt.y);
				dst = org(Rect(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height));
				virtualLoop = Rect(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height);
			}
			break;
	case true:
		if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处划圆  
		{
			pre_pt = Point(x, y);
			drawDefault(x, y, "dst", imgL, dst, pre_pt, cur_pt, 0);
			drawLineFunc();
		}
		else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划直线  
		{
			cur_pt = Point(x, y);
			drawDefault(x, y, "dst", tmpL, imgL, pre_pt, cur_pt, 2);
			//drawLineFunc();
		}
		else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图像上划直线  
		{
			Vec4i l;
			cur_pt = Point(x, y);
			drawDefault(x, y, "dst", imgL, dst, pre_pt, cur_pt, 2);
			l[0] = pre_pt.x;
			l[1] = pre_pt.y;
			l[2] = cur_pt.x;
			l[3] = cur_pt.y;
			lines.push_back(l);
			drawLineFunc();
		}
		break;

	}
	
}
//输入变量为：x，y为点击的坐标，windowName为显示窗口的名称
void drawDefault(int x, int y, string windowName, Mat windowImage, Mat preImage, Point pre_pt, Point cur_pt, int flag)
{
	char temp[16];
	preImage.copyTo(windowImage);//将原始图片复制到img中  
	sprintf(temp, "(%d,%d)", x, y);
	switch (flag)
	{
	case 1:
		rectangle(windowImage, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);
		break;
	case 2:
		line(windowImage, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点，将矩形画到img上
		break;
	default:
		break;
	}
	putText(windowImage, temp, pre_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255), 1, 8);//在图像窗口上显示坐标  
	circle(windowImage, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//划圆  
	imshow(windowName, windowImage);
}

void drawLineFunc()
{
	for (size_t i = 0; i < lines.size(); i++)
	{
		line(imgL, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点，将矩形画到img上  
		imshow("dst", imgL);
	}
}
