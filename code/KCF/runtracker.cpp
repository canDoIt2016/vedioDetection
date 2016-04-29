#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <opencv2/opencv.hpp>  
#include "kcftracker.hpp"
#include "dirent.h"
#include "boxextractor.h"
#include "histogram.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){

    if (argc > 5) return -1;

    bool HOG = true;
    bool FIXEDWINDOW = false;
    bool MULTISCALE = true;
    bool SILENT = false;
    bool LAB = false;
	VideoCapture capture;

    for(int i = 0; i < argc; i++){
        if ( strcmp (argv[i], "hog") == 0 )
            HOG = true;
        if ( strcmp (argv[i], "fixed_window") == 0 )
            FIXEDWINDOW = true;
        if ( strcmp (argv[i], "singlescale") == 0 )
            MULTISCALE = false;
        if ( strcmp (argv[i], "show") == 0 )
            SILENT = false;
        if ( strcmp (argv[i], "lab") == 0 ){
            LAB = true;
            HOG = true;
        }
        if ( strcmp (argv[i], "gray") == 0 )
            HOG = false;
    }

	 capture.open("E:\\车辆检测\\code\\KCF\\Debug\\vedio\\test.avi");
	 capture.set(CV_CAP_PROP_POS_FRAMES, 159);

    BoxExtractor extractor;//提取的帧数
    if (!capture.isOpened())
    {
        cout << "capture device failed to open!" << endl;
        return -1;
    }

    // Create KCFTracker object
    KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

    // Frame readed
    Mat frame;//当前帧
	Mat firstFrameROI, readFirstFrameROI;//第一帧的感兴趣区域
	Mat curFrameROI;//当前帧的感兴趣区域
	double similar;//当前帧与第一帧感兴趣区域的相似度
	int unMatchNum = 0;//与保存图像不匹配的数目
	double similarROI;//最初模板与当前感兴趣区域的匹配度
	Histogram1D histogram1;
	Histogram1D histogram2;
	MatND histo1;
	MatND histo2;
    // Tracker results
    Rect result;//追踪的结果矩形


    float xMin,yMin,width,height;

    // Write Results
    ofstream resultsFile;
    string resultsPath = "output.txt";
    resultsFile.open(resultsPath);

    // Frame counter
    int nFrames = 0;
    cv::namedWindow("Image");
    cv::moveWindow("Image", 100, 100);

    while (1){
        capture>>frame;
        if(frame.empty ())break;

        //double t = (double)cvGetTickCount();//记载程序运行的时间
        // First frame, give the groundtruth to the tracker
        if (nFrames == 0) {
            cv::Rect box = extractor.extract ("select",frame);
            tracker.init( box, frame );
            rectangle( frame, Point( box.x, box.y ), Point( box.x+box.width, box.y+box.height), Scalar( 0, 255, 255 ), 1, 8 );
            resultsFile << box.x << "," << box.y << "," << box.width << "," << box.height << endl;//
			firstFrameROI = frame(Rect(box.x, box.y, box.width, box.height));
			//imshow("dsagdf", firstFrameROI);
			imwrite("..//image//firstFrameCompare.jpg", firstFrameROI);//保存第一帧的比较图像
        }
        // Update
        else{
            result = tracker.update(frame);
            rectangle( frame, Point( result.x, result.y ), Point( result.x+result.width, result.y+result.height), Scalar( 0, 255, 255 ), 1, 8 );
			readFirstFrameROI = imread("..//image//firstFrameCompare.jpg");
			/*if (readFirstFrameROI.empty()) 
				cout << "未读到";*/
			curFrameROI = frame(Rect(result.x, result.y, result.width, result.height));
			/*if (curFrameROI.empty())
				cout << "未找到" << endl;
			imshow("curFrameROI", curFrameROI);*/
			//将感兴趣区域转化为直方图
			histo1 = histogram1.getHistogram(readFirstFrameROI);
			histo2 = histogram2.getHistogram(curFrameROI);
			similarROI = compareHist(histo1, histo2, CV_COMP_BHATTACHARYYA);
			cout << "similarROI=" << similarROI<<endl;
			if (similarROI > 0.6) unMatchNum++;
            resultsFile << result.x << "," << result.y << "," << result.width << "," << result.height << endl;
        }

        nFrames++;

        if (!SILENT){
            imshow("Image", frame);
            char c = waitKey(1);
            if(c==27)break;
        }

        //t = (double)cvGetTickCount() - t;
        //cout << "cost time: " << t / ((double)cvGetTickFrequency()*1000.) << endl;
		if (unMatchNum > 10)
		{
			cout << "目标丢失" << endl;
			break;
		}
			
    }
    resultsFile.close();

}
