#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "general_utils.h"
#include "mnist_convert.h"

static int write_Mnist(std::vector<cv::Mat> &vec, std::string filename)
{
	int cnt = 0;
	if(0 >= vec.size()) {
		return 0;
	}
#ifdef _PRINT_CONSOLE
	int progress = 10;
#endif
	std::ofstream file(filename, std::ios::binary);
	if (file.is_open()) {
		int magic_number = 2051;
		int number_of_images = vec.size();
		int n_rows = vec[0].rows;
		int n_cols = vec[0].cols;
		magic_number = ReverseInt(magic_number);
		file.write((char*)&magic_number, sizeof(magic_number));
		number_of_images = ReverseInt(number_of_images);
		file.write((char*)&number_of_images, sizeof(number_of_images));
		n_rows = ReverseInt(n_rows);
		file.write((char*)&n_rows, sizeof(n_rows));
		n_cols = ReverseInt(n_cols);
		file.write((char*)&n_cols, sizeof(n_cols));

		n_rows = vec[0].rows;
		n_cols = vec[0].cols;
		for(std::vector<cv::Mat>::iterator iter = vec.begin(); 
			iter != vec.end(); iter++, cnt++) {
			for (int r = 0; r < n_rows; ++r) {
				for (int c = 0; c < n_cols; ++c) {
					unsigned char temp = (*iter).at<uchar>(r,c);
					file.write((char*)&temp, sizeof(temp));
				}
			}
#ifdef _PRINT_CONSOLE
			// 显示读取进度
			if(progress <= cnt * 100 / vec.size()) {
				printf("Write MNist file progress... %d%%\n", progress);
				progress += 10;
			}
#endif
		}
		file.close();
		if(vec.size() != cnt) {
			return 0;
		}
	}
	return cnt;
}

static int write_Mnist_Label(std::vector<int> &vec, std::string filename)
{
	int cnt = 0;
	if(0 >= vec.size()) {
		return 0;
	}
	std::ofstream file(filename, std::ios::binary);
	if (file.is_open()) {
		int magic_number = 2049;
		int number_of_images = vec.size();
		magic_number = ReverseInt(magic_number);
		file.write((char*)&magic_number, sizeof(magic_number));
		number_of_images = ReverseInt(number_of_images);
		file.write((char*)&number_of_images, sizeof(number_of_images));

		for(std::vector<int>::iterator iter = vec.begin(); 
			iter != vec.end(); iter++, cnt++) {
			unsigned char temp = (unsigned char)(*iter);
			file.write((char*)&temp, sizeof(temp));
		}
		file.close();
		if(vec.size() != cnt) {
			return 0;
		}
	}
	return cnt;
}

static int ParseImageName(std::string image_path_name, int &label, int &idx)
{
	char fileName[MAX_PATH];
	if(0 >= getFileName(image_path_name.c_str(), fileName, 0)) {
#ifdef _PRINT_CONSOLE
		printf("Cannot get file name. [ParseImageName] image_path_name=%s\n", 
			image_path_name.c_str());
#endif
		return 0;
	}

	char strNum[MAX_PATH] = {0};
	int len = 0, pos = 0, num[2] = {0};
	for (int i = 0; i <= strlen(fileName); i++) {
		if('0' <= fileName[i] && '9' >= fileName[i]) {
			strNum[len++] = fileName[i];
		} else {
			strNum[len] = '\0';
			num[pos++] = atoi(strNum);
			if(1 < pos)
				break;
			strNum[0] = '\0';
			len = 0;
		}
	}

	label = num[0];
	idx = num[1];
	return 1;
}

int ImagetoMNist(std::string cv_images_path, std::vector<cv::Mat> vec_cv_images, 
				 std::vector<int> vec_n_labels)
{
	// 读入图像文件路径
	std::list<PathElem> ImgList;
	int status = findImageFiles(cv_images_path.c_str(), ImgList);
	if(0 >= status) {
#ifdef _PRINT_CONSOLE
		printf("Cannot find image file in path: %s\n", cv_images_path.c_str());
#endif
		return 0;
	}

#ifdef _PRINT_CONSOLE
	printf("Find %d image files in path: %s\n", ImgList.size(), cv_images_path.c_str());
#endif

	int imwid = 0, imhei = 0;
	vec_cv_images.clear();
	vec_n_labels.clear();

#ifdef _PRINT_CONSOLE
	int progress = 10;
#endif

	// 遍历读入vector<Mat>容器
	std::list<PathElem>::iterator pImgListTemp = ImgList.begin();
	for(int iik = 1; iik <= ImgList.size(); iik++, pImgListTemp++) {
		cv::Mat src = cv::imread(pImgListTemp->SrcImgPath, 0);
		if (src.empty()) {
#ifdef _PRINT_CONSOLE
			printf("Read image failed.[ImagetoMNist] path: %s\n", pImgListTemp->SrcImgPath);
#endif
			continue;
		} else if(1 == iik) {
			imwid = src.cols;
			imhei = src.rows;
#ifdef _PRINT_CONSOLE
			printf("Set image width: %d, height: %d\n", imwid, imhei);
#endif
		}

		if(imwid != src.cols || imhei != src.rows) {
#ifdef _PRINT_CONSOLE
			printf("Invalid image size.[ImagetoMNist] width: %d, height: %d\n", 
				src.cols, src.rows);
#endif
			continue;
		}

		std::string impath = pImgListTemp->SrcImgPath;
		int label = 0, index = 0;
		if(1 != ParseImageName(impath, label, index)) {
#ifdef _PRINT_CONSOLE
			printf("Read image label failed.[ImagetoMNist] impath: %s\n", pImgListTemp->SrcImgPath);
#endif
			continue;
		}

		vec_cv_images.push_back(src);
		vec_n_labels.push_back(label);
#ifdef _PRINT_CONSOLE
		// 显示读取进度
		if(progress <= iik * 100 / ImgList.size()) {
			printf("Read image progress... %d%%\n", progress);
			progress += 10;
		}
#endif
	}

	if(vec_cv_images.size() != vec_n_labels.size()) {
#ifdef _PRINT_CONSOLE
		printf("Size matching failed. [ImagetoMNist] images.size()=%d, labels.size=%d\n", 
			vec_cv_images.size(), vec_n_labels.size());
#endif
		return 0;
	} else {
#ifdef _PRINT_CONSOLE
		printf("Read %d(total: %d) images successfully.\n", 
			vec_cv_images.size(), ImgList.size());
#endif
	}

	//  图像数据写入MNist文件
	std::string mnist_image_path = cv_images_path + ".images-ubyte";
	if(1 != creatFile(mnist_image_path.c_str(), 0)) {
#ifdef _PRINT_CONSOLE
		printf("Create file failed.[ImagetoMNist] mnist_image_path: %s\n", 
			mnist_image_path.c_str());
#endif
		return 0;
	}

	std::string mnist_label_path = cv_images_path + ".labels-ubyte";
	if(1 != creatFile(mnist_label_path.c_str(), 0)) {
#ifdef _PRINT_CONSOLE
		printf("Create file failed.[ImagetoMNist] mnist_label_path: %s\n", 
			mnist_label_path.c_str());
#endif
		return 0;
	}

	status = write_Mnist(vec_cv_images, mnist_image_path);
	if(vec_cv_images.size() != status) {
#ifdef _PRINT_CONSOLE
		printf("Write MNist file failed.[ImagetoMNist] write_Mnist ret=%d\n", 
			status);
#endif
		return 0;
	}

	status = write_Mnist_Label(vec_n_labels, mnist_label_path);
	if(vec_n_labels.size() != status) {
#ifdef _PRINT_CONSOLE
		printf("Write MNist Label file failed.[ImagetoMNist] write_Mnist_Label ret=%d\n", 
			status);
#endif
		return 0;
	}

#ifdef _PRINT_CONSOLE
	printf("Write MNist file successfully.\n");
#endif

	ShowGalleryImage(cv_images_path, 20, 1);

	return vec_cv_images.size();
}

