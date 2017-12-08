// TemporaryTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <math.h>

#include "ryucv.h"
#include "image_ui.h"
#include "general_utils.h"

#include "mnist_convert.h"
#include "code_qal.h"
#include "code_locate.h"
#include "Exwaybill_cutout.h"


//////////////////////////////////////////////////////////////////////////
// Test begin-XXX
// Test end-XXX
//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	int status = 0;

	/*/////////////////////////////////////////////////////////////////////////
	// Test begin - mnist_convert
	std::string mnist_images_path = "F:/EN08_QRCodeRecog/20170719_mnist/t10k-images.idx3-ubyte";
	std::string mnist_labels_path = "F:/EN08_QRCodeRecog/20170719_mnist/t10k-labels.idx1-ubyte";
	std::vector<cv::Mat> vec_cv_images;
	std::vector<int> vec_n_labels;

	std::string cv_images_path = "F:/EN08_QRCodeRecog/20170719_mnist/test-images";
	status = ImagetoMNist(cv_images_path, vec_cv_images, vec_n_labels);

	status = MNISTtoImage(mnist_images_path, mnist_labels_path, 
		vec_cv_images, vec_n_labels);

	// Test end - mnist_convert
	/////////////////////////////////////////////////////////////////////////*/


	//////////////////////////////////////////////////////////////////////////
	// im_proc test
	// 图像读取路径，可以是文件夹或单个图像文件
	char srcFilePath[] = //"F:\\EN08_QRCodeRecog\\im_raw";
						 "F:\\EN08_OCRNumRecog\\20170907_整合数字截图";
	char dstPath[MAX_PATH] = "F:/EN08_QRCodeRecog/";
	std::list<PathElem> ImgList; 
	std::list<PathElem>::iterator pImgListTemp; 

	int start_frame = 150;

	CvPoint label_pt = cvPoint(-1, -1);

	int initial_flag = 0;

	// 读取图像文件/文件夹下的图像文件
	status = findImageFiles(srcFilePath, ImgList);
	if(0 >= status) {
		return 0;
	}

	// 依次读取图像文件
	pImgListTemp = ImgList.begin();
	for(int iik = 1; iik <= ImgList.size(); iik++, pImgListTemp++)
	{
		// 从指定帧号开始循环
		if(start_frame > iik) 
			continue;

		printf("\n==========================Frame:%d==========================\n", iik);
		printf("%s\n", pImgListTemp->SrcImgPath);

		IplImage * im = cvLoadImage(pImgListTemp->SrcImgPath, CV_LOAD_IMAGE_GRAYSCALE);
		if(NULL == im) {
			printf("Warn. Load image failed. [_tmain] im=0x%x\n", im);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		// Test begin-测试二维码定位算法

		// 算法测时启动
		ryuTimerStart();

		status = CodeLocating_process(im, pImgListTemp->SrcImgPath);

		// 算法测时停止
		long lTimeCost = ryuTimerStop();
		char sTimeCost[32];
		ryuThousandType(lTimeCost, sTimeCost);
		printf("--_tmain-- 算法耗时: %s us\n", sTimeCost);

		// Test end-测试二维码定位算法
		//////////////////////////////////////////////////////////////////////////

		/*/////////////////////////////////////////////////////////////////////////
		// Test begin-测试OCR图像二值化效果
		double hscale = 1.0, vscale = 1.0;
		cvNamedWindow("im");
		cvShowImage("im", im);

		RyuImage * im_ryu = iplImage2ryuImage(im);

		RyuImage * integrogram = ryuCreateImage(ryuGetSize(im_ryu), RYU_DEPTH_32N, 1);
		RyuSize masksz = ryuSize(im->height, im->height);
		RyuImage * sharpen = ryuCreateImage(ryuGetSize(im_ryu), 8, 1);
		RyuImage * binary = ryuCreateImage(ryuGetSize(im_ryu), 8, 1);

		// 算法测时启动
		ryuTimerStart();

		// 锐化处理
		ryuAutoContrast(im_ryu, im_ryu);
		ryuUsmSharpening(im_ryu, sharpen, 0, 160, 0);

		// 二值化处理
		status = ryuCentralLocalBinarizeFast(sharpen, integrogram, binary, masksz, 0.2, 1);

		// 算法测时停止
		ryuTimerStop();
		ryuTimerPrint();

		// 图像显示
		ryuShowImage("sharpen", sharpen);
		ryuShowImage("sharpen binary", binary);

		// 对比直接二值化
		status = ryuCentralLocalBinarizeFast(im_ryu, integrogram, binary, masksz, 0.15, 1);
		ryuShowImage("original binary", binary);

		ryuReleaseImageHeader(&im_ryu);

		if(integrogram)
			ryuReleaseImage(&integrogram);
		if(sharpen)
			ryuReleaseImage(&sharpen);
		if(binary)
			ryuReleaseImage(&binary);
		// Test end-测试OCR图像二值化效果
		/////////////////////////////////////////////////////////////////////////*/

		/*/////////////////////////////////////////////////////////////////////////
		// Test begin-测试相机调校模块功能
		// 图像区域标定
		if(label_pt.x < 0) {
			CvPoint rect[4];
			status = ryuLabelImageInWindow(im, 1.0, 1.0, rect, RYU_LABELIM_CROSSPOINT);
			if(1 != status) {
				printf("[_tmain] Error. Unexpected return of ryuLabelImageInWindow. ret=%d\n", status);
				printf("[_tmain] Press <esc> to exit, press other key to continue...\n");
				int wait_key = cvWaitKey();
				if(27 == wait_key)
					return 0;
				else
					continue;
			}
			label_pt = rect[0];
		}

		// 算法测时启动
		ryuTimerStart();

		int C = 0, L = 0, D = 0;
		BarcodeQualityDetect(im, label_pt, &C, &L, &D);

		// 算法测时停止
		long lTimeCost = ryuTimerStop();
		char sTimeCost[32];
		ryuThousandType(lTimeCost, sTimeCost);
		printf("--_tmain-- 算法耗时: %s us\n", sTimeCost);
		// Test end-XXX
		/////////////////////////////////////////////////////////////////////////*/

		if(im)	
			cvReleaseImage(&im);
		if(27 == cvWaitKey())
			break;
	}
	// im_proc end
	//////////////////////////////////////////////////////////////////////////

	return 0;
}

