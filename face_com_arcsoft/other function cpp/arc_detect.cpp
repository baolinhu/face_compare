#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>
#include<stdio.h>
#include<tchar.h>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include "inc/arcsoft_fsdk_face_detection.h"
#include "inc/merror.h"

#pragma comment(lib,"lib/libarcsoft_fsdk_face_detection.lib")
using namespace cv;
using namespace std;

#define WORKBUF_SIZE        (40*1024*1024)
//#define INPUT_IMAGE_PATH "2007_007763.jpg"

bool readBmp(const char* path, uint8_t **imageData, int *pWidth, int *pHeight)
{
	if (path == NULL || imageData == NULL || pWidth == NULL || pHeight == NULL)
	{
		fprintf(stderr, "ReadBmp para error\r\n");
		return false;
	}
	FILE *fp = fopen(path, "rb");
	if (fp == 0)
	{
		fprintf(stderr, "Bmp file open failed\r\n");
		return false;
	}
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);
	//cout << sizeof(BITMAPFILEHEADER) << endl;
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	*pWidth = head.biWidth;
	*pHeight = head.biHeight;
	int biBitCount = head.biBitCount;
	int lineByte = ((*pWidth) * biBitCount / 8 + 3) / 4 * 4;
	*imageData = (uint8_t *)malloc(lineByte * (*pHeight));
	for (int i = 0; i < *pHeight; i++)
	{
		fseek(fp, (*pHeight - 1 - i) * lineByte + 54, SEEK_SET);
		fread(*imageData + i * (*pWidth) * 3, 1, (*pWidth) * 3, fp);
	}
	fclose(fp);
	return true;
}
bool readJpg(const char* path, uint8_t **imageData, int *pWidth, int *pHeight)
{
	if (path == NULL || imageData == NULL || pWidth == NULL || pHeight == NULL)
	{
		fprintf(stderr, "ReadBmp para error\r\n");
		return false;
	}
	FILE *fp = fopen(path, "rb");
	if (fp == 0)
	{
		fprintf(stderr, "Bmp file open failed\r\n");
		return false;
	}
	//fseek(fp, sizeof(BITMAPFILEHEADER), 0);
	//cout << sizeof(BITMAPFILEHEADER) << endl;
	//BITMAPINFOHEADER head;
	//fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	Mat img = imread(path);
	*pWidth = img.cols;
	*pHeight = img.rows;
	int biBitCount = 24;
	int lineByte = ((*pWidth) * biBitCount / 8 ) ;
	*imageData = (uint8_t *)malloc(lineByte * (*pHeight));
	for (int i = 0; i < *pHeight; i++)
	{
		fseek(fp, (*pHeight - 1 - i) * lineByte+12 , SEEK_SET);
		fread(*imageData + i * (*pWidth) * 3, 1, (*pWidth) * 3, fp);
	}
	fclose(fp);
	return true;
}
bool MatTopImgData(Mat& img, uint8_t **imageData, int *pWidth, int *pHeight)
{
	//获取图像参数
	int row = img.rows;
	int col = img.cols;
	int band = img.channels();
	*pWidth = col;
	*pHeight = row;
	//定义图像二值指针
	int lineByte = band*col;
	*imageData = (uint8_t *)malloc(lineByte * (*pHeight));
	for (int i = 0; i<row; i++)  //行数--高度
	{
		unsigned char* data = img.ptr<unsigned char>(i); //指向第i行的数据
		for (int j = 0; j<col; j++)      //列数 -- 宽度
		{
			for (int m = 0; m<band; m++)     //将各个波段的数据存入数组
				*imageData[i*lineByte+j*band + m] = data[j*band + m];
		}
	}
	return true;
	////获取图像参数
	//int row = img.rows;
	//int col = img.cols;
	//int band = img.channels();
	//*pWidth = col;
	//*pHeight = row;
	////定义图像二值指针
	//uchar **pImgdata = new unsigned char*[row];
	//for (int i = 0; i<row; i++)
	//	pImgdata[i] = new unsigned char[band*col];

	//for (int i = 0; i<row; i++)  //行数--高度
	//{
	//	unsigned char* data = img.ptr<unsigned char>(i); //指向第i行的数据
	//	for (int j = 0; j<col; j++)      //列数 -- 宽度
	//	{
	//		for (int m = 0; m<band; m++)     //将各个波段的数据存入数组
	//			pImgdata[i][j*band+m] = data[j*band + m];
	//	}
	//}
	//return pImgdata;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//初始化
	MRESULT nRet = MERR_UNKNOWN;
	MHandle hEngine = nullptr;
	char APPID[256] = "4iYKQo9nEDx5CzyybYxtwoWDW8soLZgoGMMMgCT9skUa";
	char SDKKey[256] = "EDS9Eco96tpxXRbEqAh3AjMSPW3ceJU7zVXxxGFCBz5h";
	MInt32 nScale = 20;
	MInt32 nMaxFace = 10;
	MByte *pWorkMem = (MByte *)malloc(WORKBUF_SIZE);
	if (pWorkMem == nullptr)
	{
		fprintf(stderr, "Fail to malloc workbuf\r\n");
		return -1;
	}
	nRet = AFD_FSDK_InitialFaceEngine(APPID, SDKKey, pWorkMem, WORKBUF_SIZE, &hEngine, AFD_FSDK_OPF_0_ONLY, nScale, nMaxFace);
	if (nRet != MOK || hEngine == nullptr)
	{
		fprintf(stderr, "InitialFaceEngine failed , errorcode is %d \r\n", nRet);
		return -1;
	}
	//获取版本信息
	const AFD_FSDK_Version * pVersionInfo = nullptr;
	pVersionInfo = AFD_FSDK_GetVersion(hEngine);
	fprintf(stdout, "%d %d %d %d\r\n", pVersionInfo->lCodebase, pVersionInfo->lMajor, pVersionInfo->lMinor, pVersionInfo->lBuild);
	fprintf(stdout, "%s\r\n", pVersionInfo->Version);
	fprintf(stdout, "%s\r\n", pVersionInfo->BuildDate);
	fprintf(stdout, "%s\r\n", pVersionInfo->CopyRight);
	//读取bmp图片数据
	string filename1 = "2007_007763.jpg";
	Mat img = imread(filename1);
	imshow("原图", img);
	cv::waitKey(10);
	cout << (int)img.at<Vec3b>(100, 100)[2]   << endl;
	ASVLOFFSCREEN offInput = { 0 };
	offInput.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
	//offInput.ppu8Plane[0] = nullptr;
	//readJpg(INPUT_IMAGE_PATH, (uint8_t**)&offInput.ppu8Plane[0], &offInput.i32Width, &offInput.i32Height);
	// readBmp(INPUT_IMAGE_PATH, (uint8_t**)&offInput.ppu8Plane[0], &offInput.i32Width, &offInput.i32Height);
	offInput.ppu8Plane[0] = (uint8_t*)img.data;
	offInput.i32Height = img.rows;
	offInput.i32Width = img.cols;
	if (!offInput.ppu8Plane[0])
	{
		fprintf(stderr, "Fail to ReadBmp(%s)\r\n", filename1);
		AFD_FSDK_UninitialFaceEngine(hEngine);
		free(pWorkMem);
		return -1;
	}
	else
	{
		fprintf(stdout, "Picture width : %d , height : %d \r\n", offInput.i32Width, offInput.i32Height);
	}
	offInput.pi32Pitch[0] = offInput.i32Width * 3;
	//人脸检测
	LPAFD_FSDK_FACERES	FaceRes = nullptr;
	//for (int loop = 0; loop < 10; ++loop)
	//{
		nRet = AFD_FSDK_StillImageFaceDetection(hEngine, &offInput, &FaceRes);
		if (nRet == MOK)
		{
			fprintf(stdout, "The number of face: %d\r\n", FaceRes->nFace);
			for (int i = 0; i < FaceRes->nFace; ++i)
			{
				fprintf(stdout, "Face[%d]: rect[%d,%d,%d,%d], Face orient: %d\r\n", i, FaceRes->rcFace[i].left, FaceRes->rcFace[i].top, FaceRes->rcFace[i].right, FaceRes->rcFace[i].bottom, FaceRes->lfaceOrient[i]);
				int l = FaceRes->rcFace[i].left, t = FaceRes->rcFace[i].top, r = FaceRes->rcFace[i].right, b = FaceRes->rcFace[i].bottom;
				rectangle(img, Point(l,t),Point(r,b), Scalar(255, 0, 0), 2, 8, 0);
				imshow("结果", img);
				cv::waitKey();
			}
		}
		else
		{
			fprintf(stderr, "Face Detection failed, error code: %d\r\n", nRet);
		}
	//}
	//反初始化
	//free(offInput.ppu8Plane[0]);
	nRet = AFD_FSDK_UninitialFaceEngine(hEngine);
	if (nRet != MOK)
	{
		fprintf(stderr, "UninitialFaceEngine failed , errorcode is %d \r\n", nRet);
	}
	free(pWorkMem);
	return 0;
}