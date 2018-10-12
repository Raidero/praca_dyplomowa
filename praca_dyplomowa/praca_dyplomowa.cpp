#include "pch.h"
#include <iostream>
#include <opencv2/core/base.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <list>
#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "Encoder.h"
#include "Decoder.h"

using namespace cv;

int main(int argc, char** argv)
{
	Encoder* encoder = new Encoder();
	Decoder* decoder = new Decoder();
	Mat src;
	Mat_<double> bgr[3];
	Mat_<double> bgr2[3];
	std::string window_name = "Dyplom";
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	if (argc > 1)
	{
		src = imread(argv[1], IMREAD_ANYCOLOR);
	}
	else
	{
		src = imread("test.png", IMREAD_ANYCOLOR);
	}
	if (!src.data)
	{
		std::cerr << "File with given name does not exist\n";
		return -1;
	}
	
	src.convertTo(src, CV_64FC3);
	//cv::normalize(src, src, 0.0, 1.0, cv::NORM_MINMAX);
	src = src / 255.0;
	Mat* dst;
	Mat compressed = src.clone();
	imshow(window_name, src);
	waitKey();
	//cv::Mat_<double>* channels = new cv::Mat_<double>[src.channels()];
	//split(src, channels);
	encoder->compressAndSave(src, "img.wt");
	/*encoder->maxValue = new double[3];
	encoder->minValue = new double[3];
	for (int i = 0; i < src.channels(); ++i)
	{
		const String type = cv::typeToString(channels[i].type());
		encoder->transform(channels[i]);
		encoder->normalizeValues(channels[i], i);
	}
	Mat part = channels[0](cv::Rect2d(0, 0, 16, 16));
	cout << part;*/
	//cv::Mat* channels2 = new cv::Mat[src.channels()];
	///*for (int i = 0; i < src.channels(); ++i)
	//{
	//	channels2[i] = cv::Mat(src.rows, src.cols, CV_64F);
	//	channels[i].forEach([](cv::Pixel &pixel, const int * position) -> void
	//	{
	//		complicatedThreshold(pixel);
	//	})
	//}*/

	////for (int i = 0; i < src.channels(); ++i)
	////	encoder->transform(channels[i]);

	//for (int i = 0; i < src.channels(); ++i)
	//	decoder->inverseTransform(channels[i]);
	dst = decoder->loadAndDecompress("img.wt");
	//const String type = cv::typeToString(dst->type());
	//merge(channels, src.channels(), src);
	imshow(window_name, *dst);
	waitKey();

	return 0;
}

//void normalize(cv::Mat& src, double h, double l)
//{
//	for (int level = 0; level < LEVEL; ++level)
//	{
//		for (int i = 0; i < (src.rows >> level); ++i)
//		{
//			for (int j = 0; j < (src.cols >> (level + 1)); ++j)
//			{
//				src.at<double>(i, j) /= l;
//				src.at<double>(i, j + (src.cols >> (level + 1))) /= h;
//			}
//		}
//		for (int i = 0; i < (src.rows >> (level + 1)); ++i)
//		{
//			for (int j = 0; j < (src.cols >> level); ++j)
//			{
//				src.at<double>(i, j) /= l;
//				src.at<double>(i + (src.rows >> (level + 1)), j) /= h;
//			}
//		}
//	}
//}
//
//double *temp = NULL;
//
//void fwt97NoMove(double* x, int n)
//{
//	double a;
//
//	a = -1.586134342;
//	for (int i = 1; i < n - 2; i += 2)
//	{
//		x[i] += a * (x[i - 1] + x[i + 1]);
//	}
//	x[n - 1] += 2 * a * x[n - 2];
//
//	a = -0.05298011854;
//	for (int i = 2; i < n; i += 2)
//	{
//		x[i] += a * (x[i - 1] + x[i + 1]);
//	}
//	x[0] += 2 * a * x[1];
//
//	a = 0.8829110762;
//	for (int i = 1; i < n - 2; i += 2)
//	{
//		x[i] += a * (x[i - 1] + x[i + 1]);
//	}
//	x[n - 1] += 2 * a * x[n - 2];
//
//	a = 0.4435068522;
//	for (int i = 2; i < n; i += 2)
//	{
//		x[i] += a * (x[i - 1] + x[i + 1]);
//	}
//	x[0] += 2 * a * x[1];
//
//	a = 1 / 1.149604398;
//	for (int i = 0; i < n; ++i)
//	{
//		if (i % 2 == 1)
//			x[i] *= a;
//		else
//			x[i] /= a;
//	}
//}