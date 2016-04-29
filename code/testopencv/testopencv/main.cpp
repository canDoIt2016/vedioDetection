#include <iostream>  
#include <stdio.h>
#include "histogram.h"
#include "opencv2/opencv.hpp"  
#include <opencv2/nonfree/features2d.hpp>
using namespace cv;
using namespace std;

//void readme();

/** @function main */
int main(int argc, char** argv)
{
	/*if (argc != 3)
	{
		readme(); return -1;
	}*/
	Mat gray1, gray2;
	Mat img_1 = imread("..//image//supercar.jpg");
	Mat imageROI1 = img_1(Rect(160, 270, 200, 200));
	Mat imageROI2 = img_1(Rect(160, 270, 200, 200));
	cvtColor(imageROI1, gray1, CV_RGB2GRAY);
	cvtColor(imageROI2, gray2, CV_RGB2GRAY);
	imshow("test1", gray1);
	imshow("test2",gray2);
	Histogram1D histogram1;
	Histogram1D histogram2;
	cv::MatND histo1 = histogram1.getHistogram(gray1);
	cv::MatND histo2 = histogram2.getHistogram(gray2);
	/*for (int i = 0; i<256; i++)
		cout << "Value " << i << " = " << histo1.at<float>(i) << endl;
	for (int i = 0; i<256; i++)
		cout << "Value " << i << " = " << histo2.at<float>(i) << endl;*/
	// Display a histogram as an image
	cv::namedWindow("Histogram1");
	cv::imshow("Histogram1", histogram1.getHistogramImage(gray1));
	cv::namedWindow("Histogram2");
	cv::imshow("Histogram2", histogram2.getHistogramImage(gray2));
	double similar = compareHist(histo1, histo2, CV_COMP_BHATTACHARYYA);
	cout << similar << endl;
	//Mat img_2 = imread("..//image//supercar2.jpg");

	//if (!img_1.data || !img_2.data)
	//{
	//	std::cout << " --(!) Error reading images " << std::endl; return -1;
	//}

	////-- Step 1: Detect the keypoints using SURF Detector
	//int minHessian = 400;

	//SurfFeatureDetector detector(minHessian);

	//std::vector<KeyPoint> keypoints_1, keypoints_2;

	//detector.detect(img_1, keypoints_1);
	//detector.detect(img_2, keypoints_2);

	////-- Step 2: Calculate descriptors (feature vectors)
	//SurfDescriptorExtractor extractor;

	//Mat descriptors_1, descriptors_2;

	//extractor.compute(img_1, keypoints_1, descriptors_1);
	//extractor.compute(img_2, keypoints_2, descriptors_2);

	////-- Step 3: Matching descriptor vectors using FLANN matcher
	//FlannBasedMatcher matcher;
	//std::vector< DMatch > matches;
	//matcher.match(descriptors_1, descriptors_2, matches);

	//double max_dist = 0; double min_dist = 100;

	////-- Quick calculation of max and min distances between keypoints
	//for (int i = 0; i < descriptors_1.rows; i++)
	//{
	//	double dist = matches[i].distance;
	//	if (dist < min_dist) min_dist = dist;
	//	if (dist > max_dist) max_dist = dist;
	//}

	////printf("-- Max dist : %f \n", max_dist);
	////printf("-- Min dist : %f \n", min_dist);

	////-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
	////-- PS.- radiusMatch can also be used here.
	//std::vector< DMatch > good_matches;
	//for (int i = 0; i < descriptors_1.rows; i++)
	//{
	//	if (matches[i].distance < 3 * min_dist)
	//	{
	//		good_matches.push_back(matches[i]);
	//	}
	//}

	////-- Draw only "good" matches
	//Mat img_matches;
	//drawMatches(img_1, keypoints_1, img_2, keypoints_2,
	//	good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
	//	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	////-- Show detected matches
	//imshow("Good Matches", img_matches);

	//for (int i = 0; i < good_matches.size(); i++)
	//{
	//	printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	//}

	waitKey(0);

	return 0;
}

///** @function readme */
//void readme()
//{
//	std::cout << " Usage: ./SURF_FlannMatcher <img1> <img2>" << std::endl;
//}