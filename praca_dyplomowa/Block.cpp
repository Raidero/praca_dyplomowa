#include "pch.h"
#include "Block.h"

Block::Block()
{
	energy = 0;
	order = 0;
	data = new vector<double>();
}

Block::~Block()
{
	delete data;
}

void Block::computeProperties(cv::Mat& src, int offsetX, int offsetY, int zoneMaxX, int zoneMaxY, int blockSize, int order)
{
	for (int i = 0; i < blockSize; ++i)
	{
		for (int j = 0; j < blockSize; ++j)
		{
			if (i + offsetX < zoneMaxX && j + offsetY < zoneMaxY)
			{
				const double value = src.at<double>(i + offsetX, j + offsetY);
				energy += value*value;
				data->push_back(value);
			}
		}
	}
	this->order = order;
}

void Block::saveToFile(ofstream& file)
{
	file.write((char*)&order, sizeof(int));
	for (int i = 0; i < data->size(); ++i)
	{
		char value = (char)(data->at(i) * 255);
		file.write((char*)&value, sizeof(char));
	}
}

double Block::GetEnergy() const 
{ 
	return energy; 
}

int Block::GetOrder() const
{
	return order;
}


