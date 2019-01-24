#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include "Block.h"
#include "WaveletTransform.h"

using namespace std;

class Encoder : public WaveletTransform
{
	public:
		Encoder();
		~Encoder();
		void compressAndSave(cv::Mat & src, string fileName);
		
		void setWalvetSeries(int series);
		void setBlockSize(int blockSize);
		void setBitRate(double compressionLevel);
		double GetDataEnergy();
		
	private:
		int walvetSeries;
		int blockSize;
		double bitRate;
		vector<Block*>* blockData;
		int numberOfBlocks;
		vector<bool>* bitMap;

		void transform(cv::Mat& src);
		void horizontalTransformation(cv::Mat & src, int level);
		void verticalTransformation(cv::Mat & src, int level);
		void createFileAndSaveInitialInformation(cv::Mat& src, string filename);
		void saveData(string fileName);
		void convertRGBtoYCbCr(cv::Mat & src);
		void computePropertiesForAllBlocks(cv::Mat & src);
		void reduceNumberOfBlocksByBitRate(cv::Mat& src);
		void sortBlocksByEnergy();
		void sortBlocksByOrder();
		void createBitMap();
		void saveBitMap(string fileName);
};

