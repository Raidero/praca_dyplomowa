#include "pch.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <list>
#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "Compressor.h"

using namespace cv;

int main(int argc, char** argv)
{
	Compressor* compressor = new Compressor();
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
		src = imread("test3.png", IMREAD_ANYCOLOR);
	}
	if (!src.data)
	{
		std::cerr << "File with given name does not exist\n";
		return -1;
	}
	//////////// liczymy współczynniki
	//double highPass[] = { 0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0 };
	//double lowPass[] = { 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0 };
	//double coefficentHighPass = 0;
	//double coefficentLowPass = 0;
	//fwt97NoMove(highPass, 16);
	//fwt97NoMove(lowPass, 16);
	//for (int i = 0; i < 16; ++i)
	//{
	//	coefficentLowPass += lowPass[i] * lowPass[i];
	//	coefficentHighPass += highPass[i] * highPass[i];
	//}
	//std::cout << coefficentHighPass << '\n';
	//std::cout << coefficentLowPass << '\n';
	//////////
	src.convertTo(src, CV_64FC3);
	src = src / 255.0;

	Mat compressed = src.clone();
	imshow(window_name, src);
	waitKey();
	compressor->setWalvetSeries(3);
	switch (src.channels())
	{
	case 1:
		compressor->compressAndSave(src, "img.wt");
		imshow(window_name, src);
		waitKey();
		//compressor->computePropertiesForAllBlocks(src);
		//cout << compressor->GetDataEnergy() << endl;
		//normalize(src, coefficentHighPass, coefficentLowPass);
		//cout << energy(src) << endl;
		/*energyForBlocks(src, BLOCK_SIZE);
		imshow(window_name, src);
		waitKey();*/

		//compressor->inverseTransform(src);
		//imshow(window_name, src);
		//waitKey();
		break;
	case 3:
		split(src, bgr);
		compressor->transform(bgr[0]);
		compressor->transform(bgr[1]);
		compressor->transform(bgr[2]);
		merge(bgr, 3, src);

		imshow(window_name, src);
		waitKey();



		split(src, bgr);
		compressor->inverseTransform(bgr[0]);
		compressor->inverseTransform(bgr[1]);
		compressor->inverseTransform(bgr[2]);
		merge(bgr, 3, src);

		imshow(window_name, src);
		waitKey();
		break;
	}

	/*switch (src.channels())
	{
	case 1:
		compress(src, compressed, 4);

		imshow(window_name, compressed);
		waitKey();

		decompress(compressed, src, 4);

		imshow(window_name, src);
		waitKey(0);
		break;
	case 3:
		split(src, bgr);
		split(src, bgr2);
		compress(bgr[0], bgr2[0], 4);
		compress(bgr[1], bgr2[1], 4);
		compress(bgr[2], bgr2[2], 4);
		merge(bgr2, 3, src);

		imshow(window_name, src);
		waitKey();

		split(src, bgr);
		decompress(bgr[0], bgr2[0], 4);
		decompress(bgr[1], bgr2[1], 4);
		decompress(bgr[2], bgr2[2], 4);
		merge(bgr2, 3, src);

		imshow(window_name, src);
		waitKey();
		break;
	}*/

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