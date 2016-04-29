#pragma once  
#include <iostream>  
#include <stdio.h>
#include "opencv2/opencv.hpp"  
#include <opencv2/nonfree/features2d.hpp>



using namespace cv;
using namespace std;

class MorphoFeatures
{

private:
	int maxCorners = 1000;
	int maxTrackbar = 100;
	//RNG rng;
public:
	void cornersTest(Mat image1,Mat image2)
	{
		//Mat cornerImage;
		//// vector of keyPoints  
		//vector<KeyPoint> keyPoints;
		//// construction of the fast feature detector object  
		//FastFeatureDetector fast(40);   // ¼ì²âµÄãÐÖµÎª40  
		//// feature point detection  
		//fast.detect(image, keyPoints);
		//return keyPoints;
		////cout<<keyPoints.size()<<endl;
		////drawKeypoints(image, keyPoints, outImage, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_OVER_OUTIMG);
		////imshow("FAST feature", outImage);
		//-- Step 1: Detect the keypoints using SURF Detector
		int minHessian = 400;
		SurfFeatureDetector detector(minHessian);

		std::vector<KeyPoint> keypoints_1, keypoints_2;

		detector.detect(image1, keypoints_1);
		detector.detect(image2, keypoints_2);

		//-- Step 2: Calculate descriptors (feature vectors)
		SurfDescriptorExtractor extractor;

		Mat descriptors_1, descriptors_2;

		extractor.compute(image1, keypoints_1, descriptors_1);
		extractor.compute(image2, keypoints_2, descriptors_2);

		//-- Step 3: Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match(descriptors_1, descriptors_2, matches);
		double max_dist = 0; double min_dist = 100;

		//-- Quick calculation of max and min distances between keypoints
		for (int i = 0; i < descriptors_1.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		//printf("-- Max dist : %f \n", max_dist);
		//printf("-- Min dist : %f \n", min_dist);

		//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
		//-- PS.- radiusMatch can also be used here.
		std::vector< DMatch > good_matches;

		for (int i = 0; i < descriptors_1.rows; i++)
		{
			if (matches[i].distance < 10 * min_dist)
			{
				good_matches.push_back(matches[i]);
			}
		}
		//-- Draw only "good" matches
		Mat img_matches;
		drawMatches(image1, keypoints_1, image2, keypoints_2,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		//-- Show detected matches
		imshow("Good Matches", img_matches);
		//waitKey(1);
	}

};
