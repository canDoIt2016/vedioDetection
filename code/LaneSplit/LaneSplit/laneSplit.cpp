#include "opencv2/opencv.hpp"  
#include <iostream>
#include "ViBe.h"  

using namespace cv;
using namespace std;

Mat org, dst, img, tmp;
Mat imgL, dstL, tmpL;
bool drawLine = false;
Rect virtualLoop;
vector<Vec4i> lines;//�޶������м���İ��ߴ�ŵ�λ��
int laneNum;//Ҫ�����ĳ�����
void on_mouse(int event, int x, int y, int flags, void *ustc);
void drawLineFunc();
void drawDefault(int x, int y, string windowName, Mat windowImage, Mat preImage, Point pre_pt, Point cur_pt, int flag);
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
			imshow("dst", dst);
			cout << "������׼��������ֱ������" ;
			cin >> laneNum;
			drawLine = true;
			dst.copyTo(imgL);
			dst.copyTo(tmpL);
			/*for (int i = 0; i < laneNum; i++)
			{*/
				setMouseCallback("dst", on_mouse, 0);//���ûص�����,��ֱ��
			//}
			waitKey(0);
			
			//imshow("dst", dstL);
		}
		else
		{
			lines;
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
	switch (drawLine)
	{
	case false:
		if (event == CV_EVENT_LBUTTONDOWN)//������£���ȡ��ʼ���꣬����ͼ���ϸõ㴦��Բ  
			{
				pre_pt = Point(x, y);
				drawDefault(x, y, "img", img, org, pre_pt, cur_pt, 0);
			}
			else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//�������ʱ������ƶ�������ͼ���ϻ�����  
			{
				cur_pt = Point(x, y);
				drawDefault(x, y, "img", img, tmp, pre_pt, cur_pt, 1);
			}
			else if (event == CV_EVENT_LBUTTONUP)//����ɿ�������ͼ���ϻ�����  
			{
				cur_pt = Point(x, y);
				drawDefault(x, y, "img", img, org, pre_pt, cur_pt, 1);
				img.copyTo(tmp);
				//��ȡ���ΰ�Χ��ͼ�񣬲����浽dst��  
				int width = abs(pre_pt.x - cur_pt.x);
				int height = abs(pre_pt.y - cur_pt.y);
				dst = org(Rect(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height));
				virtualLoop = Rect(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height);
			}
			break;
	case true:
		if (event == CV_EVENT_LBUTTONDOWN)//������£���ȡ��ʼ���꣬����ͼ���ϸõ㴦��Բ  
		{
			pre_pt = Point(x, y);
			drawDefault(x, y, "dst", imgL, dst, pre_pt, cur_pt, 0);
			drawLineFunc();
		}
		else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//�������ʱ������ƶ�������ͼ���ϻ�ֱ��  
		{
			cur_pt = Point(x, y);
			drawDefault(x, y, "dst", tmpL, imgL, pre_pt, cur_pt, 2);
			//drawLineFunc();
		}
		else if (event == CV_EVENT_LBUTTONUP)//����ɿ�������ͼ���ϻ�ֱ��  
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
//�������Ϊ��x��yΪ��������꣬windowNameΪ��ʾ���ڵ�����
void drawDefault(int x, int y, string windowName, Mat windowImage, Mat preImage, Point pre_pt, Point cur_pt, int flag)
{
	char temp[16];
	preImage.copyTo(windowImage);//��ԭʼͼƬ���Ƶ�img��  
	sprintf(temp, "(%d,%d)", x, y);
	switch (flag)
	{
	case 1:
		rectangle(windowImage, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);
		break;
	case 2:
		line(windowImage, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//���ݳ�ʼ��ͽ����㣬�����λ���img��
		break;
	default:
		break;
	}
	putText(windowImage, temp, pre_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255), 1, 8);//��ͼ�񴰿�����ʾ����  
	circle(windowImage, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//��Բ  
	imshow(windowName, windowImage);
}

void drawLineFunc()
{
	for (size_t i = 0; i < lines.size(); i++)
	{
		line(imgL, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 255, 0, 0), 1, 8, 0);//���ݳ�ʼ��ͽ����㣬�����λ���img��  
		imshow("dst", imgL);
	}
}
