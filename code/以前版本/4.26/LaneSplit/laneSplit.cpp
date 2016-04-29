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
	Mat element5(3, 3, CV_8U, Scalar(1));//�������еĽṹ��
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
			cout << "����" << endl;
			break;
		}
		
		if (count==1)
		{
			org.copyTo(img);
			org.copyTo(tmp);
			imshow("img", img);
			//�趨��������
			setMouseCallback("img", on_mouse, 0);//���ûص�����,������
			waitKey(0);
			//namedWindow("dst");
			imshow("dst", dst);
			setMouseCallback("img", on_mouse, 0);//���ûص�����,������
			
		}
		else
		{
			if (count==2)
			{
				cvtColor(frame(virtualLoop), gray, CV_RGB2GRAY);//��֡��ͼ��ҶȻ�
				//vibe��ʼ������ͼ��
				Vibe_Bgs.init(gray);
				Vibe_Bgs.processFirstFrame(gray);
				cout << " Training GMM complete!" << endl;
			}
			else
			{
				cvtColor(frame(virtualLoop), gray, CV_RGB2GRAY);//��֡��ͼ��ҶȻ�
				Vibe_Bgs.testAndUpdate(gray);
				mask = Vibe_Bgs.getMask();
				morphologyEx(mask, mask, MORPH_CLOSE, element5);//���б����㽫��������ͨ����
				cout << endl << "x:" << virtualLoop.x << "y:" << virtualLoop.y<<"with:"<<virtualLoop.width<<"height:"<<virtualLoop.height<<endl;
				imshow("img", mask);
			}
			
		}
		
		waitKey(10);
	}
	return 0;
}

void on_mouse(int event, int x, int y, int flags, void *ustc)//event����¼����ţ�x,y������꣬flags��ק�ͼ��̲����Ĵ���  
{
	static Point pre_pt = (-1, -1);//��ʼ����  
	static Point cur_pt = (-1, -1);//ʵʱ����  
	char temp[16];
	if (event == CV_EVENT_LBUTTONDOWN)//������£���ȡ��ʼ���꣬����ͼ���ϸõ㴦��Բ  
	{
		org.copyTo(img);//��ԭʼͼƬ���Ƶ�img��  
		sprintf(temp, "(%d,%d)", x, y);
		pre_pt = Point(x, y);
		putText(img, temp, pre_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255), 1, 8);//��ͼ�񴰿�����ʾ����  
		circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//��Բ  
		imshow("img", img);
	}
	else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//���û�а��µ����������ƶ��Ĵ�����  
	{
		img.copyTo(tmp);//��img���Ƶ���ʱͼ��tmp�ϣ�������ʾʵʱ����  
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));//ֻ��ʵʱ��ʾ����ƶ�������  
		imshow("img", tmp);
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//�������ʱ������ƶ�������ͼ���ϻ�����  
	{
		img.copyTo(tmp);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
		line(tmp, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//����ʱͼ����ʵʱ��ʾ����϶�ʱ�γɵľ���  
		imshow("img", tmp);
	}
	else if (event == CV_EVENT_LBUTTONUP)//����ɿ�������ͼ���ϻ�����  
	{
		org.copyTo(img);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(img, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
		circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
		line(img, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//���ݳ�ʼ��ͽ����㣬�����λ���img��  
		imshow("img", img);
		img.copyTo(tmp);
		//��ȡ���ΰ�Χ��ͼ�񣬲����浽dst��  
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