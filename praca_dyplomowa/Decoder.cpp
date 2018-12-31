#include "pch.h"
#include "Decoder.h"


Decoder::Decoder()
{
	walvetSeries = 0;
	blockSize = 0;
	counter = 0;
	order = -1;
	channels = nullptr;
	numberOfChannels = 1;
	numberOfBlocks = 0;
	bitMap = new vector<bool>();
}


Decoder::~Decoder()
{
	if (channels != nullptr)
	{
		delete[] channels;
	}
}

cv::Mat* Decoder::loadAndDecompress(string fileName)
{
	ifstream file(fileName, ofstream::in | ofstream::binary);

	loadInitialInformation(file);
	loadBitMap(file);
	counter = 0;

	for (int i = 0; i < numberOfChannels; ++i)
	{
		loadData(channels[i], file, i);
		inverseTransform(channels[i]);
		channels[i] += 0.5;
	}

	file.close();

	cv::Mat* src = new cv::Mat;
	cv::merge(channels, numberOfChannels, *src);
	convertYCbCrToRGB(*src);

	return src;
}

void Decoder::inverseTransform(cv::Mat& src)
{
	for (int level = walvetSeries - 1; level >= 0; --level)
	{
		verticalInverseTransformation(src, level);
		horizontalInverseTransformation(src, level);
	}
}

void Decoder::horizontalInverseTransformation(cv::Mat& src, int level)
{
	vector<double> data(src.cols >> level);
	for (int i = 0; i < (src.rows >> level); ++i)
	{
		for (int j = 0; j < (src.cols >> level); ++j)
		{
			data[j] = src.at<double>(i, j);
		}
		inverseWalvetTranform97(data);
		for (int j = 0; j < (src.cols >> level); ++j)
		{
			src.at<double>(i, j) = data[j];
		}
	}
}

void Decoder::verticalInverseTransformation(cv::Mat& src, int level)
{
	vector<double> data(src.rows >> level);
	for (int i = 0; i < (src.cols >> level); ++i)
	{
		for (int j = 0; j < (src.rows >> level); ++j)
		{
			data[j] = src.at<double>(j, i);
		}
		inverseWalvetTranform97(data);
		for (int j = 0; j < (src.rows >> level); ++j)
		{
			src.at<double>(j, i) = data[j];
		}
	}
}

void Decoder::loadInitialInformation(ifstream& file)
{
	int rows, cols;
	// schema
	// number of channels (colors) 32 bit
	// width 32 bit
	// height 32 bit
	// block size 32 bit
	// walvet series 32 bit
	// minimal value 64 bit
	// maximal value 64 bit
	file.read((char*)&numberOfChannels, sizeof(int));
	file.read((char*)&rows, sizeof(int));
	file.read((char*)&cols, sizeof(int));

	channels = new cv::Mat[numberOfChannels];
	for (int i = 0; i < numberOfChannels; ++i)
	{
		channels[i] = cv::Mat(rows, cols, CV_64FC1);
	}

	file.read((char*)&blockSize, sizeof(int));
	file.read((char*)&walvetSeries, sizeof(int));
	file.read((char*)&numberOfBlocks, sizeof(int));
}

void Decoder::loadData(cv::Mat& src, ifstream& file, int channel)
{
	Block block;

	for (int i = 0; i < (src.rows >> walvetSeries); i += blockSize)
	{
		for (int j = 0; j < (src.cols >> walvetSeries); j += blockSize)
		{
			block.loadToMat(src, i, j, (src.rows >> walvetSeries), (src.cols >> walvetSeries), blockSize, file, bitMap->at(counter), walvetSeries);
			counter++;
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
					block.loadToMat(src, i + offsetX, j + offsetY, offsetX + zoneWidth, offsetY + zoneHeight, blockSize, file, bitMap->at(counter), level);
					counter++;
				}
			}
		}
	}
}

void Decoder::convertYCbCrToRGB(cv::Mat& src)
{
	if (src.channels() == 3)
	{
		cv::Mat bgr[3];
		cv::Mat ycbcr[3];
		cv::split(src, ycbcr);

		bgr[0] = ycbcr[0] + 1.773 * (ycbcr[1] - 0.5);
		bgr[1] = ycbcr[0] - 0.714 * (ycbcr[2] - 0.5) - 0.344 * (ycbcr[1] - 0.5);
		bgr[2] = ycbcr[0] + 1.403 * (ycbcr[2] - 0.5);

		cv::merge(bgr, 3, src);
	}
}

void Decoder::loadBitMap(ifstream& file)
{
	bitMap->resize(numberOfBlocks);
	char byte = 0;
	for (int i = 0; i < numberOfBlocks; ++i)
	{
		if (i % 8 == 0)
		{
			file.read((char*)&byte, sizeof(char));
		}
		if (byte & (1 << (i % 8)))
		{
			bitMap->at(i) = true;
		}
	}
}