#pragma once
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include "Block.h"

using namespace std;

class Compressor
{
	public:
		Compressor();
		~Compressor();
		void transform(cv::Mat & src);
		void inverseTransform(cv::Mat & src);
		void horizontalTransformation(cv::Mat & src, int level, bool inverse = false);
		void verticalTransformation(cv::Mat & src, int level, bool inverse = false);
		void setWalvetSeries(int series);
		void setBlockSize(int blockSize);
		void setCompressionLevel(int compressionLevel);
		double GetDataEnergy();
		void computePropertiesForAllBlocks(cv::Mat & src);
	private:
		int walvetSeries;
		int blockSize;
		int compressionLevel;
		vector<Block>* blockData;

		void forwardWalvetTransform97(vector<double>& values);
		void inverseWalvetTranform97(vector<double>& values);

		void predictionStep(vector<double>& values, const double coefficient);
		void updateStep(vector<double>& values, const double coefficient);

		void normalizeStep(vector<double>& values);
		void denormalizeStep(vector<double>& values);

		

		void reduceNumberOfBlocksByLevelOfCompression();

		void sortBlocksByEnergy();

		void sortBlocksByOrder();

		

		void orderValues(vector<double>& values);
		void disorderValues(vector<double>& values);

};

