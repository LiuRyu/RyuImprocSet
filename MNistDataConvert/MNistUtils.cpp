#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "general_utils.h"
#include "mnist_convert.h"


int ReverseInt(int i)
{
	unsigned char ch1, ch2, ch3, ch4;
	ch1 = i & 255;
	ch2 = (i >> 8) & 255;
	ch3 = (i >> 16) & 255;
	ch4 = (i >> 24) & 255;
	return((int)ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}

int ShowGalleryImage(std::string cv_images_path, int cols, int is_save)
{
	int width = 28 * cols;
	int height = 28 * 10;
	cv::Mat gallery(height, width, CV_8UC1);

	int len = cv_images_path.size();
	if(0 >= len) {
#ifdef _PRINT_CONSOLE
		printf("Invalid image path. [ShowGalleryImage] cv_images_path=%s\n", 
			cv_images_path.c_str());
#endif
		return 0;
	}

	std::string im_path = cv_images_path;
	// ÐÞÕýÂ·¾¶
	if('\\' != im_path[len-1] && '/' != im_path[len-1]) {
		im_path = im_path + getPathSeparator(cv_images_path.c_str());
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 1; j <= cols; j++) {
			int x = (j-1) * 28;
			int y = i * 28;
			cv::Mat part = gallery(cv::Rect(x, y, 28, 28));

			std::string str = std::to_string(j);
			if (j < 10)
				str = "0000" + str;
			else
				str = "000" + str;

			str = std::to_string(i) + "_" + str + ".jpg";
			std::string input_image = im_path + str;

			cv::Mat src = cv::imread(input_image, 0);
			if (src.empty()) {
#ifdef _PRINT_CONSOLE
				printf("Read image failed. [ShowGalleryImage] images_path=%s\n", 
					input_image.c_str());
#endif
				return -1;
			}

			src.copyTo(part);
		}
	}

	if(is_save) {
		std::string output_image = im_path + "gallery/gallery.png";
		cv::imwrite(output_image, gallery);
	}
	cv::namedWindow("gallery");
	cv::imshow("gallery", gallery);
	cv::waitKey();
}


