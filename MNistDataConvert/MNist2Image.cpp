#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "general_utils.h"
#include "mnist_convert.h"


static int read_Mnist(std::string filename, std::vector<cv::Mat> &vec)
{
	int ret = 0;
#ifdef _PRINT_CONSOLE
	int progress = 10;
#endif
	std::ifstream file(filename, std::ios::binary);
	if (file.is_open()) {
		int magic_number = 0;
		int number_of_images = 0;
		int n_rows = 0;
		int n_cols = 0;
		file.read((char*)&magic_number, sizeof(magic_number));
		magic_number = ReverseInt(magic_number);
		file.read((char*)&number_of_images, sizeof(number_of_images));
		number_of_images = ReverseInt(number_of_images);
		file.read((char*)&n_rows, sizeof(n_rows));
		n_rows = ReverseInt(n_rows);
		file.read((char*)&n_cols, sizeof(n_cols));
		n_cols = ReverseInt(n_cols);

		for (int i = 0; i < number_of_images; ++i) {
			cv::Mat tp = cv::Mat::zeros(n_rows, n_cols, CV_8UC1);
			for (int r = 0; r < n_rows; ++r) {
				for (int c = 0; c < n_cols; ++c) {
					unsigned char temp = 0;
					file.read((char*)&temp, sizeof(temp));
					tp.at<uchar>(r, c) = (int)temp;
				}
			}
			vec.push_back(tp);
#ifdef _PRINT_CONSOLE
			// 显示读取进度
			if(progress <= (i+1) * 100 / number_of_images) {
				printf("Read MNist progress... %d%%\n", progress);
				progress += 10;
			}
#endif
		}
		file.close();
		ret = number_of_images;
	}
	return ret;
}

static int read_Mnist_Label(std::string filename, std::vector<int> &vec)
{
	int ret = 0;
	std::ifstream file(filename, std::ios::binary);
	if (file.is_open()) {
		int magic_number = 0;
		int number_of_images = 0;
		int n_rows = 0;
		int n_cols = 0;
		file.read((char*)&magic_number, sizeof(magic_number));
		magic_number = ReverseInt(magic_number);
		file.read((char*)&number_of_images, sizeof(number_of_images));
		number_of_images = ReverseInt(number_of_images);

		for (int i = 0; i < number_of_images; ++i) {
			unsigned char temp = 0;
			file.read((char*)&temp, sizeof(temp));
			//vec[i] = (int)temp;
			vec.push_back((int)temp);
		}
		file.close();
		ret = number_of_images;
	}
	return ret;
}

static std::string GetImageName(int number, int arr[])
{
	std::string str1, str2;

	for (int i = 0; i < 10; i++) {
		if (number == i) {
			arr[i]++;
			str1 = std::to_string(arr[i]);

			if (arr[i] < 10) {
				str1 = "0000" + str1;
			} else if (arr[i] < 100) {
				str1 = "000" + str1;
			} else if (arr[i] < 1000) {
				str1 = "00" + str1;
			} else if (arr[i] < 10000) {
				str1 = "0" + str1;
			}

			break;
		}
	}

	str2 = std::to_string(number) + "_" + str1;

	return str2;
}

int MNISTtoImage(std::string mnist_images_path, std::string mnist_labels_path, 
				 std::vector<cv::Mat> vec_cv_images, std::vector<int> vec_n_labels)
{
	int ret = 0;
	int status = isFileExist(mnist_images_path.c_str());
	if(TRUE != status) {
#ifdef _PRINT_CONSOLE
		printf("File not found. [MNISTtoImage] mnist_images_path=%s\n", 
			mnist_images_path.c_str());
#endif
		return -1;
	}

	status = isFileExist(mnist_labels_path.c_str());
	if(TRUE != status) {
#ifdef _PRINT_CONSOLE
		printf("File not found. [MNISTtoImage] mnist_labels_path=%s\n", 
			mnist_labels_path.c_str());
#endif
		return -1;
	}


#ifdef _PRINT_CONSOLE
	printf("Reading image data in file: %s\n", mnist_images_path.c_str());
#endif

	vec_cv_images.clear();
	status = read_Mnist(mnist_images_path, vec_cv_images);
	if(0 >= status) {
#ifdef _PRINT_CONSOLE
		printf("Unexpected return. [MNISTtoImage] read_Mnist ret=%d\n", status);
#endif
		return -1;
	}

	vec_n_labels.clear();
	status = read_Mnist_Label(mnist_labels_path, vec_n_labels);
	if(0 >= status) {
#ifdef _PRINT_CONSOLE
		printf("Unexpected return. [MNISTtoImage] read_Mnist_Label ret=%d\n", 
			status);
#endif
		return -1;
	}

	if (vec_cv_images.size() != vec_n_labels.size()) {
#ifdef _PRINT_CONSOLE
		printf("Size not match. [MNISTtoImage] images count=%d, label count=%d\n", 
			vec_cv_images.size(), vec_n_labels.size());
#endif
		return -1;
	}

#ifdef _PRINT_CONSOLE
	printf("Read %d images from MNist successfully.\n", 
		vec_cv_images.size());
#endif

	// save images
	int count_digits[10];
	std::fill(&count_digits[0], &count_digits[0] + 10, 0);

	char im_path[MAX_PATH], im_name[MAX_PATH], sp[2] = {0}; 
	getParentPath(mnist_images_path.c_str(), im_path);
	getFileName(mnist_images_path.c_str(), im_name, 0);
	sp[0] = getPathSeparator(mnist_images_path.c_str());
	strcat_s(im_path, sp);
	strcat_s(im_path, im_name);
	strcat_s(im_path, "-cv");
	strcat_s(im_path, sp);
	std::string save_images_path = im_path;

	status = creatFlieDir(save_images_path.c_str());
	if(1 != status) {
#ifdef _PRINT_CONSOLE
		printf("Cannot creat file dir. [MNISTtoImage]\n", 
			vec_cv_images.size(), vec_n_labels.size());
#endif
		return -1;
	}

#ifdef _PRINT_CONSOLE
	int progress = 10;
#endif
	for (int i = 0; i < vec_cv_images.size(); i++) {
		int number = vec_n_labels[i];
		std::string image_name = GetImageName(number, count_digits);
		image_name = save_images_path + image_name + ".jpg";
		cv::imwrite(image_name, vec_cv_images[i]);
#ifdef _PRINT_CONSOLE
		// 显示读取进度
		if(progress <= (i+1) * 100 / vec_cv_images.size()) {
			printf("Write image files progress... %d%%\n", progress);
			progress += 10;
		}
#endif
// 		cv::namedWindow("im");
// 		cv::imshow("im", vec_cv_images[i]);
// 		cv::waitKey();
	}

#ifdef _PRINT_CONSOLE
	printf("Write %d image files successfully.\n", vec_cv_images.size());
#endif

	ShowGalleryImage(save_images_path, 20, 1);


	ret = vec_cv_images.size();

	return ret;
}

