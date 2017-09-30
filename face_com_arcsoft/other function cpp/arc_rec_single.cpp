#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>
#include<stdio.h>
#include<tchar.h>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<fstream>
#include<time.h>
#include "recinc/arcsoft_fsdk_face_recognition.h"
#include "recinc/merror.h"
#include "detinc/arcsoft_fsdk_face_detection.h"
#include "detinc/merror.h"

#pragma comment(lib,"lib/libarcsoft_fsdk_face_detection.lib")
#pragma comment(lib,"lib/libarcsoft_fsdk_face_recognition.lib")
using namespace cv;
using namespace std;

#define WORKBUF_SIZE        (40*1024*1024)
//static ofstream fout("result35.txt", ios::ate | ios::out | ios::app);//| ios::app 保留原来的内容
//#define INPUT_IMAGE1_PATH "test_image/sfz/00岑进伟.bmp"
//#define INPUT_IMAGE2_PATH "test_image/640/1.bmp"

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
int _tmain(int argc, _TCHAR* argv[])
{
	//初始化
	string path1 = "./test_image/sfz/26.bmp";
	string path2 = "./test_image/face_bmp1/559.bmp";
	MRESULT nRet = MERR_UNKNOWN;
	MHandle hEngine1 = nullptr;
	MHandle hEngine2 = nullptr;
	char APPID[256] = "4iYKQo9nEDx5CzyybYxtwoWDW8soLZgoGMMMgCT9skUa";
	char SDKKey1[256] = "EDS9Eco96tpxXRbEqAh3AjMSPW3ceJU7zVXxxGFCBz5h";
	char SDKKey2[256] = "EDS9Eco96tpxXRbEqAh3AjMZYuJkwN2wgeTEiqBsjhUH";
	MInt32 nScale = 16;
	MInt32 nMaxFace = 10;
	MByte *pWorkMem1 = (MByte *)malloc(WORKBUF_SIZE);
	MByte *pWorkMem2 = (MByte *)malloc(WORKBUF_SIZE);
	if (pWorkMem1 == nullptr)
	{
		fprintf(stderr, "fail to malloc workbuf\r\n");
		return -1;
	}
	nRet = AFD_FSDK_InitialFaceEngine(APPID, SDKKey1, pWorkMem1, WORKBUF_SIZE, &hEngine1, AFD_FSDK_OPF_0_ONLY, nScale, nMaxFace);
	nRet = AFR_FSDK_InitialEngine(APPID, SDKKey2, pWorkMem2, WORKBUF_SIZE, &hEngine2);
	if (nRet != MOK || hEngine2 == nullptr)
	{
		fprintf(stderr, "InitialFaceEngine failed , errorcode is %d \r\n", nRet);
		return -1;
	}
	//获取版本信息
	const AFR_FSDK_Version * pVersionInfo = nullptr;
	pVersionInfo = AFR_FSDK_GetVersion(hEngine2);
	fprintf(stdout, "%d %d %d %d %d\r\n", pVersionInfo->lCodebase, pVersionInfo->lMajor, pVersionInfo->lMinor, pVersionInfo->lBuild, pVersionInfo->lFeatureLevel);
	fprintf(stdout, "%s\r\n", pVersionInfo->Version);
	fprintf(stdout, "%s\r\n", pVersionInfo->BuildDate);
	fprintf(stdout, "%s\r\n", pVersionInfo->CopyRight);

	//读取第一张图片
	ASVLOFFSCREEN offInput1 = { 0 };
	//cout << path1 <<  endl;
	Mat img1 = imread(path1);
	imshow("img1", img1);
	cv::waitKey(8);

	offInput1.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
	offInput1.ppu8Plane[0] = nullptr;
	readBmp(path1.c_str(), (uint8_t**)&offInput1.ppu8Plane[0], &offInput1.i32Width, &offInput1.i32Height);
	if (!offInput1.ppu8Plane[0])
	{
		fprintf(stderr, "fail to ReadBmp(%s)\r\n", path1);
		AFR_FSDK_UninitialEngine(hEngine2);
		free(pWorkMem1);
		free(pWorkMem2);
		return -1;
	}
	offInput1.pi32Pitch[0] = offInput1.i32Width * 3;

	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	{
		AFR_FSDK_FACEINPUT faceResult;
		//第一张人脸信息通过face detection\face tracking获得

		LPAFD_FSDK_FACERES	FaceRes = nullptr;
		nRet = AFD_FSDK_StillImageFaceDetection(hEngine1, &offInput1, &FaceRes);
		if (nRet == MOK)
		{

			//fprintf(stdout, "The number of face: %d\r\n", FaceRes->nFace);
			for (int i = 0; i < FaceRes->nFace; ++i)
			{
				faceResult.lOrient = FaceRes->lfaceOrient[i];//人脸方向
				faceResult.rcFace.left = FaceRes->rcFace[i].left;//人脸框位置
				faceResult.rcFace.top = FaceRes->rcFace[i].top;
				faceResult.rcFace.right = FaceRes->rcFace[i].right;
				faceResult.rcFace.bottom = FaceRes->rcFace[i].bottom;
			}
		}

		//提取第一张人脸特征
		AFR_FSDK_FACEMODEL LocalFaceModels = { 0 };
		nRet = AFR_FSDK_ExtractFRFeature(hEngine2, &offInput1, &faceResult, &LocalFaceModels);
		if (nRet != MOK)
		{
			fprintf(stderr, "fail to Extract 1st FR Feature, error code: %d\r\n", nRet);
		}
		faceModels1.lFeatureSize = LocalFaceModels.lFeatureSize;
		faceModels1.pbFeature = (MByte*)malloc(faceModels1.lFeatureSize);
		memcpy(faceModels1.pbFeature, LocalFaceModels.pbFeature, faceModels1.lFeatureSize);
	}
	//读取第二张图片
	ASVLOFFSCREEN offInput2 = { 0 };
	
	offInput2.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
	offInput2.ppu8Plane[0] = nullptr;
	readBmp(path2.c_str(), (uint8_t**)&offInput2.ppu8Plane[0], &offInput2.i32Width, &offInput2.i32Height);
	if (!offInput2.ppu8Plane[0])
	{
		fprintf(stderr, "fail to ReadBmp(%s)\r\n", path2);
		free(offInput2.ppu8Plane[0]);
		AFR_FSDK_UninitialEngine(hEngine2);
		free(pWorkMem1);
		free(pWorkMem2);
		return -1;
	}
	offInput2.pi32Pitch[0] = offInput2.i32Width * 3;
	AFR_FSDK_FACEMODEL faceModels2 = { 0 };
	{
		AFR_FSDK_FACEINPUT faceResult;
		//第二张人脸信息通过face detection\face tracking获得
		LPAFD_FSDK_FACERES	FaceRes = nullptr;
		nRet = AFD_FSDK_StillImageFaceDetection(hEngine1, &offInput2, &FaceRes);
		if (nRet == MOK)
		{

			//fprintf(stdout, "The number of face: %d\r\n", FaceRes->nFace);
			for (int i = 0; i < FaceRes->nFace; ++i)
			{
				faceResult.lOrient = FaceRes->lfaceOrient[i];//人脸方向
				faceResult.rcFace.left = FaceRes->rcFace[i].left;//人脸框位置
				faceResult.rcFace.top = FaceRes->rcFace[i].top;
				faceResult.rcFace.right = FaceRes->rcFace[i].right;
				faceResult.rcFace.bottom = FaceRes->rcFace[i].bottom;
			}
		}
		//提取第二张人脸特征
		AFR_FSDK_FACEMODEL LocalFaceModels = { 0 };
		nRet = AFR_FSDK_ExtractFRFeature(hEngine2, &offInput2, &faceResult, &LocalFaceModels);
		if (nRet != MOK)
		{
			fprintf(stderr, "fail to Extract 2nd FR Feature, error code: %d\r\n", nRet);
		}
		faceModels2.lFeatureSize = LocalFaceModels.lFeatureSize;
		faceModels2.pbFeature = (MByte*)malloc(faceModels2.lFeatureSize);
		memcpy(faceModels2.pbFeature, LocalFaceModels.pbFeature, faceModels2.lFeatureSize);
	}
	//对比两张人脸特征，获得比对结果
	MFloat  fSimilScore = 0.0f;
	nRet = AFR_FSDK_FacePairMatching(hEngine2, &faceModels1, &faceModels2, &fSimilScore);
	if (nRet == MOK)
	{
		fprintf(stdout, "fSimilScore =  %f\r\n", fSimilScore);
	}
	else
	{
		fprintf(stderr, "FacePairMatching failed , errorcode is %d \r\n", nRet);
	}
	//反初始化
	free(offInput1.ppu8Plane[0]);
	free(offInput2.ppu8Plane[0]);
	nRet = AFR_FSDK_UninitialEngine(hEngine2);
	nRet = AFD_FSDK_UninitialFaceEngine(hEngine1);
	if (nRet != MOK)
	{
		fprintf(stderr, "UninitialFaceEngine failed , errorcode is %d \r\n", nRet);
	}
	free(pWorkMem1);
	free(pWorkMem2);
	free(faceModels1.pbFeature);
	free(faceModels2.pbFeature);
	return 0;
}

