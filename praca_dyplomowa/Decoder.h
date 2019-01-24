#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <list>
#include "Block.h"
#include "WaveletTransform.h"


class Decoder : public WaveletTransform
{
	public:
		Decoder();
		~Decoder();

		cv::Mat* loadAndDecompress(string fileName);
		
	private:
		int walvetSeries;
		int blockSize;
		int numberOfChannels;
		int counter; // order counter
		int order;
		int numberOfBlocks;
		vector<bool>* bitMap;
		cv::Mat* channels;
		void inverseTransform(cv::Mat& src);
		void horizontalInverseTransformation(cv::Mat& src, int level);
		void verticalInverseTransformation(cv::Mat& src, int level);

		void loadInitialInformation(ifstream& file);

		void loadData(cv::Mat& src, ifstream& file, int channel);

		void convertYCbCrToRGB(cv::Mat & src);

		void loadBitMap(ifstream & file);

};

