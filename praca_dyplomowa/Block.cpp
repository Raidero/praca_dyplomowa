#include "pch.h"
#include "Block.h"

Block::Block()
{
	energy = 0;
	order = 0;
	width = 0;
	height = 0;
	data = new vector<double>();
	defaultValue = 0;
	bitShift = 0;
}

Block::Block(int order, vector<double>* data)
{
	this->order = order;
	this->data = data;
	energy = 0;
	defaultValue = 0;
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
			else if (i + offsetX == zoneMaxX)
			{
				width = i;
			} 
			else if (j + offsetY == zoneMaxY)
			{
				height = j;
			}
		}
	}
	this->order = order;
	height = height ? height : blockSize;
	width = width ? width : blockSize;
}

void Block::saveToFile(ofstream& file)
{
	for (int i = 0; i < data->size(); ++i)
	{
		double doubleValueAfterShift = (data->at(i) + (1 << (bitShift - 1)));
		if (doubleValueAfterShift < 0)
		{
			doubleValueAfterShift = 0;
		}
		unsigned short shortValue = (unsigned short)(doubleValueAfterShift * 1024.0);
		file.write((char*)&shortValue, sizeof(unsigned short));
	}
}

void Block::setBitShift(int shift)
{
	bitShift = shift;
}

void Block::loadToMat(cv::Mat& src, int offsetX, int offsetY, int zoneMaxX, int zoneMaxY, int blockSize, ifstream& file, bool loaded, int shift)
{
	if (loaded) 
	{
		for (int i = 0; i < blockSize; ++i)
		{
			for (int j = 0; j < blockSize; ++j)
			{
				if (i + offsetX < zoneMaxX && j + offsetY < zoneMaxY)
				{
					unsigned short value;
					file.read((char*)&value, sizeof(unsigned short));
					src.at<double>(i + offsetX, j + offsetY) = ((double)value / 1024.0) - (1 << (shift - 1));
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < blockSize; ++i)
		{
			for (int j = 0; j < blockSize; ++j)
			{
				if (i + offsetX < zoneMaxX && j + offsetY < zoneMaxY)
				{
					src.at<double>(i + offsetX, j + offsetY) = 0;
				}
			}
		}
	}
}

double Block::GetEnergy() const 
{ 
	return energy / (double)(width * height); 
}

int Block::GetOrder() const
{
	return order;
}
