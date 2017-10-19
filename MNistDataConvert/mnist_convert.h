#ifndef __MNIST_DATA_CONVERT_H__
#define __MNIST_DATA_CONVERT_H__

#ifdef MNISTDATACONVERT_EXPORTS
#define MNISTDATACONVERT_EXPORTS __declspec(dllexport)
#else
#define MNISTDATACONVERT_EXPORTS __declspec(dllimport)
#endif

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

/****************************************************************************************\
*									MNIST utilities								         *
\****************************************************************************************/
/* Reverse int. */
int ReverseInt(int i);

/* Show (and save) gallery image. */
int ShowGalleryImage(std::string cv_images_path, int cols, int is_save);


/****************************************************************************************\
*									MNIST to Image								         *
\****************************************************************************************/
/* convert MNist data to cv image, save image and show. */
MNISTDATACONVERT_EXPORTS int MNISTtoImage(std::string mnist_images_path, 
										  std::string mnist_labels_path, 
										  std::vector<cv::Mat> vec_cv_images, 
										  std::vector<int> vec_n_labels);


/****************************************************************************************\
*									Image to MNIST								         *
\****************************************************************************************/
/* convert cv image to MNist data, save MNist file. */
MNISTDATACONVERT_EXPORTS int ImagetoMNist(std::string cv_images_path, 
										  std::vector<cv::Mat> vec_cv_images, 
										  std::vector<int> vec_n_labels);



#endif  __MNIST_DATA_CONVERT_H__


