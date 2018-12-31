#pragma once

#include <vector>
#include <list>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>

using namespace std;

class Block
{
	public:
		Block();
		Block(int order, vector<double>* data);
		~Block();
		double GetEnergy() const;
		int GetOrder() const;
		void computeProperties(cv::Mat& src, int offsetX, int offsetY, int zoneMaxX, int zoneMaxY, int blockSize, int order);
		void saveToFile(ofstream & file);
		void setBitShift(int shift);
		void loadToMat(cv::Mat& src, int offsetX, int offsetY, int zoneMaxX, int zoneMaxY, int blockSize, ifstream& file, bool loaded, int shift);
	private:
		int order;
		int width, height;
		int bitShift;
		double energy;
		double defaultValue;
		vector<double>* data;
};

