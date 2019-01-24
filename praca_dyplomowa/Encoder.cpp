#include "pch.h"
#include "Encoder.h"
#include <fstream>

Encoder::Encoder()
{
	walvetSeries = 1;
	blockSize = 32;
	blockData = new vector<Block*>();
	bitRate = 24;
	numberOfBlocks = 0;
	bitMap = new vector<bool>();
}


Encoder::~Encoder()
{
	for (int i = 0; i < blockData->size(); ++i)
	{
		delete blockData->at(i);
	}
	delete blockData;
	delete bitMap;
}

void Encoder::compressAndSave(cv::Mat& src, string fileName)
{
	cv::Mat_<double>* channels = new cv::Mat_<double>[src.channels()];
	
	convertRGBtoYCbCr(src);
	split(src, channels);
	
	for(int i = 0; i < src.channels(); ++i)
	{
		channels[i] -= 0.5;
		transform(channels[i]);
		computePropertiesForAllBlocks(channels[i]);
	}
	
	merge(channels, src.channels(), src);
	delete[] channels;

  	sortBlocksByEnergy();
	numberOfBlocks = blockData->size();
	reduceNumberOfBlocksByBitRate(src);

	sortBlocksByOrder();
	createFileAndSaveInitialInformation(src, fileName);
	createBitMap();
	saveBitMap(fileName);
	saveData(fileName);
}

void Encoder::transform(cv::Mat& src)
{
	for (int level = 0; level < walvetSeries; ++level)
	{
		horizontalTransformation(src, level);
		verticalTransformation(src, level);
	}
}

void Encoder::horizontalTransformation(cv::Mat& src, int level)
{
	vector<double> data(src.cols >> level);
	for (int i = 0; i < (src.rows >> level); ++i)
	{

		for (int j = 0; j < (src.cols >> level); ++j)
		{
			data[j] = src.at<double>(i, j);
		}
		forwardWalvetTransform97(data);
		for (int j = 0; j < (src.cols >> level); ++j)
		{
			src.at<double>(i, j) = data[j];
		}
	}
}

void Encoder::verticalTransformation(cv::Mat& src, int level)
{
	vector<double> data(src.rows >> level);
	for (int i = 0; i < (src.cols >> level); ++i)
	{
		for (int j = 0; j < (src.rows >> level); ++j)
		{
			data[j] = src.at<double>(j, i);
		}
		forwardWalvetTransform97(data);
		for (int j = 0; j < (src.rows >> level); ++j)
		{
			src.at<double>(j, i) = data[j];
		}
	}
}

void Encoder::setWalvetSeries(int walvetSeries)
{
	if (walvetSeries > 0)
	{
		this->walvetSeries = walvetSeries;
	}
	else
	{
		throw std::out_of_range("Number of walvet series must be a positive integer number");
	}
}

void Encoder::setBlockSize(int blockSize)
{
	if (blockSize > 0)
	{
		this->blockSize = blockSize;
	}
	else
	{
		throw std::out_of_range("Block size must be a positive integer number");
	}
}

void Encoder::setBitRate(double bitRate)
{
	if (bitRate >= 0.0 && bitRate <= 24.0)
	{
		this->bitRate = bitRate;
	}
	else
	{
		throw std::out_of_range("Bit rate must be a double value between 0 and 24");
	}
}

void Encoder::computePropertiesForAllBlocks(cv::Mat& src)
{
	for (int i = 0; i < (src.rows >> walvetSeries); i += blockSize)
	{
		for (int j = 0; j < (src.cols >> walvetSeries); j += blockSize)
		{
			Block* block = new Block();
			block->computeProperties(src, i, j, (src.rows >> walvetSeries), (src.cols >> walvetSeries), blockSize, (int)blockData->size());
			block->setBitShift(walvetSeries);
			blockData->push_back(block);
		}
	}
	for (int level = walvetSeries; level >= 1; --level)
	{
		const int zoneWidth = src.rows >> level;
		const int zoneHeight = src.cols >> level;
		for (int zone = 1; zone < 4; ++zone)
		{
			const int offsetX = (zone % 2) * zoneWidth;
			const int offsetY = (zone >> 1) * zoneHeight;
			for (int i = 0; i < zoneWidth; i += blockSize)
			{
				for (int j = 0; j < zoneHeight; j += blockSize)
				{
					Block* block = new Block();
					block->computeProperties(src, i + offsetX, j + offsetY, offsetX + zoneWidth, offsetY + zoneHeight, blockSize, (int)blockData->size());
					block->setBitShift(level);
					blockData->push_back(block);
				}
			}
		}
	}
}

void Encoder::reduceNumberOfBlocksByBitRate(cv::Mat& src)
{
	int sizeOfBitMap = (src.total() * src.channels()) / (blockSize*blockSize*8);
	int blockByteSize = blockSize * blockSize * 2;
	int headerSize = 30;
	int allowedSize = bitRate * src.total() / 8;
	int numberOfBlocks = (allowedSize - headerSize - sizeOfBitMap) / blockByteSize;
	for (int i = blockData->size() - 1; i > numberOfBlocks; --i)
	{
		delete blockData->at(i);
	}
	blockData->resize(numberOfBlocks);
}

void Encoder::sortBlocksByEnergy()
{
	sort(blockData->begin(), blockData->end(),
		[](const Block* b1, const Block* b2) -> bool { return b1->GetEnergy() > b2->GetEnergy(); });
}

void Encoder::sortBlocksByOrder()
{
	sort(blockData->begin(), blockData->end(),
		[](const Block* b1, const Block* b2) -> bool { return b1->GetOrder() < b2->GetOrder(); });
}

double Encoder::GetDataEnergy()
{
	double sum = 0;
	for (int i = 0; i < blockData->size(); ++i)
	{
		sum += blockData->at(i)->GetEnergy();
	}
	return sum;
}

void Encoder::createFileAndSaveInitialInformation(cv::Mat& src, string fileName)
{
	ofstream file(fileName, ofstream::out | ofstream::binary);
	// schema
	// number of channels (colors) 32 bit
	// width 32 bit
	// height 32 bit
	// block size 32 bit
	// walvet series 32 bit
	int channels = src.channels();

	file.write((char*)&channels, sizeof(int));
	file.write((char*)&src.rows, sizeof(int));
	file.write((char*)&src.cols, sizeof(int));
	file.write((char*)&blockSize, sizeof(int));
	file.write((char*)&walvetSeries, sizeof(int));
	file.write((char*)&numberOfBlocks, sizeof(int));

	file.close();
}

void Encoder::saveData(string fileName)
{
	ofstream file(fileName, ofstream::out | ofstream::binary | ofstream::app);
	for (int i = 0; i < blockData->size(); ++i)
	{
		blockData->at(i)->saveToFile(file);
	}
	file.close();
}

void Encoder::convertRGBtoYCbCr(cv::Mat& src)
{
	if (src.channels() == 3)
	{
		cv::Mat bgr[3];
		cv::Mat ycbcr[3];
		cv::split(src, bgr);

		ycbcr[0] = 0.299 * bgr[2] + 0.587 * bgr[1] + 0.114 * bgr[0];
		ycbcr[1] = (bgr[0] - ycbcr[0]) * 0.564 + 0.5;
		ycbcr[2] = (bgr[2] - ycbcr[0]) * 0.713 + 0.5;

		cv::merge(ycbcr, 3, src);
	}
}

void Encoder::createBitMap()
{
	bitMap->resize(numberOfBlocks);
	for (int i = 0; i < blockData->size(); ++i)
	{
		bitMap->at(blockData->at(i)->GetOrder()) = true;
	}
}

void Encoder::saveBitMap(string fileName)
{
	ofstream file(fileName, ofstream::out | ofstream::binary | ofstream::app);
	char byte = 0;
	int i;
	for (i = 0; i < bitMap->size(); ++i)
	{
		if (bitMap->at(i))
		{
			byte += 1 << (i % 8);
		}
		if (i % 8 == 7) 
		{
			file.write((char*)&byte, sizeof(char));
			byte = 0;
		}
	}
	
	if (i % 8 != 0)
	{
		file.write((char*)&byte, sizeof(char));
	}
	file.close();
}
