#include "pch.h"
#include <iostream>
#include "Encoder.h"
#include "Decoder.h"
#include <cmath>

using namespace cv;

void countAndShowPSNR(Mat& base, Mat& compressed);

int main(int argc, char** argv)
{
	Encoder* encoder = new Encoder();
	Decoder* decoder = new Decoder();
	Mat src, clone;
	bool showImage = false;
	//schema
	//argv[1] = filename
	//argv[2] = compressionLevel
	//argv[3] = walvetSeries
	//argv[4] = blockSize
	//argv[5] = showImage

	switch (argc) 
	{
	case 1:
		std::cerr << "File name not specified\n";
		return -2;
	case 6:
		showImage = (bool)atoi(argv[5]);
	case 5:
		encoder->setCompressionLevel(atoi(argv[2]));
		encoder->setWalvetSeries(atoi(argv[3]));
		encoder->setBlockSize(atoi(argv[4]));
	case 2:
		src = imread(argv[1], IMREAD_ANYCOLOR);
		break;
	default:
		std::cerr << "Wrong number of arguments\n";
		return -3;
	}

	if (!src.data)
	{
		std::cerr << "File with given name does not exist\n";
		return -1;
	}
	
	src.convertTo(src, CV_64FC3);
	normalize(src, src, 0, 1, cv::NORM_MINMAX);
	clone = src.clone();

	std::string filename(argv[1]);
	filename.append(".wt");
	encoder->compressAndSave(src, filename);
	Mat* dst = decoder->loadAndDecompress(filename);

	if (showImage)
	{
		std::string window_name = "Dyplom";
		namedWindow(window_name, CV_WINDOW_NORMAL);
		imshow(window_name, *dst);
		waitKey();
	}
	
	countAndShowPSNR(clone, *dst);

	delete encoder;
	delete decoder;

	return 0;
}

void countAndShowPSNR(Mat& base, Mat& compressed)
{
	normalize(base, base, 0, 255, cv::NORM_MINMAX);
	base.convertTo(base, CV_8UC3);
	cv::cvtColor(base, base, cv::COLOR_BGR2YCrCb);

	normalize(compressed, compressed, 0, 255, cv::NORM_MINMAX);
	compressed.convertTo(compressed, CV_8UC3);
	cv::cvtColor(compressed, compressed, cv::COLOR_BGR2YCrCb);
	
	const int size = base.rows * base.cols;
	double* mes = new double[base.channels()];
	double* psnr = new double[base.channels()];
	for (int i = 0; i < base.channels(); ++i)
	{
		mes[i] = 0;
	}

	for (int i = 0; i < base.rows; ++i)
	{
		for (int j = 0; j < base.cols; ++j)
		{
			Vec3b originalPixel = base.at<Vec3b>(i, j);
			Vec3b compressedPixel = compressed.at<Vec3b>(i, j);
			for (int c = 0; c < base.channels(); ++c)
			{
				mes[c] += (originalPixel[c] - compressedPixel[c])*(originalPixel[c] - compressedPixel[c]);
			}
		}
	}

	for (int i = 0; i < base.channels(); ++i)
	{
		mes[i] /= (double)size;
		psnr[i] = (double)(10 * log10(255 * 255 / mes[i]));
		std::cout << psnr[i] << std::endl;
	}

	delete[] psnr;
	delete[] mes;
}