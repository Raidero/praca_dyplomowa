#include "pch.h"
#include "Block.h"

Block::Block()
{
	energy = 0;
	order = 0;
	data = new vector<double>();
}

Block::Block(const Block& other)
{
	this->energy = other.energy;
	this->order = other.order;
	this->data = new vector<double>(*other.data);
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

double Block::GetEnergy() const 
{ 
	return energy; 
}

int Block::GetOrder() const
{
	return order;
}


