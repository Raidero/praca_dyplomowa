#include "pch.h"
#include "Decoder.h"


Decoder::Decoder()
{
	walvetSeries = 0;
	blockSize = 0;
	counter = 0;
	order = -1;
	minValue = nullptr;
	maxValue = nullptr;
	channels = nullptr;
}


Decoder::~Decoder()
{
	if (minValue != nullptr)
	{
		delete minValue;
	}
	if (maxValue != nullptr)
	{
		delete maxValue;
	}
	if (channels != nullptr)
	{
		delete[] channels;
	}
}

cv::Mat* Decoder::loadAndDecompress(string fileName)
{
	ifstream file(fileName, ofstream::in | ofstream::binary);

	loadInitialInformation(file);

	counter = 0;
	file.read((char*)&order, sizeof(int));

	for (int i = 0; i < numberOfChannels; ++i)
	{
		loadData(channels[i], file, i);
		denormalizeValues(channels[i], i);
		inverseTransform(channels[i]);
	}

	file.close();

	cv::Mat* src = new cv::Mat;
	cv::merge(channels, numberOfChannels, *src);

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

	minValue = new double[numberOfChannels];
	maxValue = new double[numberOfChannels];
	channels = new cv::Mat[numberOfChannels];
	for (int i = 0; i < numberOfChannels; ++i)
	{
		channels[i] = cv::Mat(rows, cols, CV_64FC1);
	}

	file.read((char*)&blockSize, sizeof(int));
	file.read((char*)&walvetSeries, sizeof(int));

	for (int i = 0; i < numberOfChannels; ++i)
	{
		file.read((char*)&minValue[i], sizeof(double));
		file.read((char*)&maxValue[i], sizeof(double));
	}
}

void Decoder::loadData(cv::Mat& src, ifstream& file, int channel)
{
	Block block;
	block.setDefaultValue(-minValue[channel] / (maxValue[channel] - minValue[channel]));

	for (int i = 0; i < (src.rows >> walvetSeries); i += blockSize)
	{
		for (int j = 0; j < (src.cols >> walvetSeries); j += blockSize)
		{
			if (order == counter)
			{
				block.loadToMat(src, i, j, (src.rows >> walvetSeries), (src.cols >> walvetSeries), blockSize, file, true);
				file.read((char*)&order, sizeof(int));
			}
			else
			{
				block.loadToMat(src, i, j, (src.rows >> walvetSeries), (src.cols >> walvetSeries), blockSize, file, false);
			}
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
					if (order == counter)
					{
						block.loadToMat(src, i + offsetX, j + offsetY, offsetX + zoneWidth, offsetY + zoneHeight, blockSize, file, true);
						file.read((char*)&order, sizeof(int));
					}
					else
					{
						block.loadToMat(src, i + offsetX, j + offsetY, offsetX + zoneWidth, offsetY + zoneHeight, blockSize, file, false);
					}
					counter++;
				}
			}
		}
	}
}

void Decoder::denormalizeValues(cv::Mat& src, int channel)
{
	src *= (maxValue[channel] - minValue[channel]);
	src += minValue[channel];

}