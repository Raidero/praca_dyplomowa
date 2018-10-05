#pragma once

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>

using namespace std;

class Block
{
	public:
		Block();
		Block(const Block& other);
		~Block();
		double GetEnergy() const;
		int GetOrder() const;
		void computeProperties(cv::Mat & src, int offsetX, int offsetY, int zoneMaxX, int zoneMaxY, int blockSize, int order);
		void saveToFile(ofstream & file);
	private:
		int order;
		double energy;
		vector<double>* data;
};

