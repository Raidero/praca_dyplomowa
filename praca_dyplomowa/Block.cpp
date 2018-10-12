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
	file.write((char*)&order, sizeof(int));
	for (int i = 0; i < data->size(); ++i)
	{
		//unsigned char value = (unsigned char)(data->at(i) * 255);
		//if (data->at(i) > 1 || data->at(i) < 0) std::cout << data->at(i) << '\n';
		double value = data->at(i);
		file.write((char*)&value, sizeof(double));
	}
}

void Block::loadToMat(cv::Mat& src, int offsetX, int offsetY, int zoneMaxX, int zoneMaxY, int blockSize, ifstream& file, bool loaded)
{
	if (loaded) 
	{
		for (int i = 0; i < blockSize; ++i)
		{
			for (int j = 0; j < blockSize; ++j)
			{
				if (i + offsetX < zoneMaxX && j + offsetY < zoneMaxY)
				{
					double value;
					file.read((char*)&value, sizeof(double));
					src.at<double>(i + offsetX, j + offsetY) = (double)value;// / 255.0;
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
					src.at<double>(i + offsetX, j + offsetY) = defaultValue;
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

void Block::setDefaultValue(double defaultValue) 
{ 
	//unsigned char val = (unsigned char)(defaultValue * 255);
	this->defaultValue = defaultValue; // (double)val / 255.0;
}
