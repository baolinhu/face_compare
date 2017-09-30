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
#include "inirw.h"
#include "inc/arcsoft_fsdk_face_recognition.h"
#include "inc/merror.h"
#include "inc/arcsoft_fsdk_face_detection.h"


#pragma comment(lib,"lib/libarcsoft_fsdk_face_detection.lib")
#pragma comment(lib,"lib/libarcsoft_fsdk_face_recognition.lib")
using namespace cv;
using namespace std;

#define WORKBUF_SIZE        (40*1024*1024)
static ofstream fout("不相关.txt", ios::ate | ios::out |ios::app);//| ios::app 保留原来的内容
static ofstream fouterror("不相关error.txt", ios::ate | ios::out | ios::app);//| ios::app 保留原来的内容
const char* fileName1 = "./test_image/sfz/";		//身份证文件夹
const char* fileName2 = "./test_image/face_bmp1/";	//人脸图像文件夹
// LPCWSTR转string
std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

// 利用winWIN32_FIND_DATA读取文件下的文件名
void readImgNamefromFile(const char* fileName, vector <string> &imgNames)
{
	// vector清零 参数设置
	imgNames.clear();
	WIN32_FIND_DATA file;
	int i = 0;
	char tempFilePath[MAX_PATH + 1];
	char tempFileName[50];
	// 转换输入文件名
	sprintf_s(tempFilePath, "%s/*", fileName);
	// 多字节转换
	WCHAR   wstr[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, tempFilePath, -1, wstr, sizeof(wstr));
	// 查找该文件待操作文件的相关属性读取到WIN32_FIND_DATA
	HANDLE handle = FindFirstFile(wstr, &file);
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindNextFile(handle, &file);
		FindNextFile(handle, &file);
		// 循环遍历得到文件夹的所有文件名    
		do
		{
			sprintf(tempFileName, "%s", fileName);
			imgNames.push_back(WChar2Ansi(file.cFileName));
			imgNames[i].insert(0, tempFileName);
			i++;
		} while (FindNextFile(handle, &file));
	}
	FindClose(handle);
}
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
LPWSTR ConvertCharToLPWSTR(const char * szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	return lpszPath;
}
double ReadConfigini(string filepath,const char *key)
{
	char *sect=0;
	double value=0;
	iniFileLoad(filepath.c_str());
	value=iniGetDouble(sect, key, value);
	return value;
}
int ReadConfiginiInt(string filepath,const char *key)
{
	char *sect = 0;
	int value = 0;
	iniFileLoad(filepath.c_str());
	value = iniGetInt(sect, key, value);
	return value;
}
int _tmain(int argc, _TCHAR* argv[])
{
	//初始化
	double thresh = 0.45;	//设置的相似度阈值
	int renum = 30;	//重复执行的次数
	const char *inipath = "config.ini";
	char *key1 = "thresh";;
	char *key2 = "repeat";;
	double temp = ReadConfigini(inipath, key1);
	int retemp = ReadConfiginiInt(inipath, key2);
	if (temp<1 && temp>0)
		thresh = temp;
	if (retemp > 0)
		renum = retemp;
	cout << "现在执行的是不相关人员的测试，\n即不同人的身份证和人脸对比.\n这里是随机选取一张身份证和一张人脸测试\n";
	//cout << "请输入阈值(建议在0.35—0.5之间)，按enter结束\n";
	//cin >> thresh;

	cout << "本次检测阈值为  "<<thresh << endl;
	cout << "本次检测随机对比  " << renum << "  组" << endl;
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
	//const AFR_FSDK_Version * pVersionInfo = nullptr;
	//pVersionInfo = AFR_FSDK_GetVersion(hEngine2);
	//fprintf(stdout, "%d %d %d %d %d\r\n", pVersionInfo->lCodebase, pVersionInfo->lMajor, pVersionInfo->lMinor, pVersionInfo->lBuild, pVersionInfo->lFeatureLevel);
	//fprintf(stdout, "%s\r\n", pVersionInfo->Version);
	//fprintf(stdout, "%s\r\n", pVersionInfo->BuildDate);
	//fprintf(stdout, "%s\r\n", pVersionInfo->CopyRight);

	std::vector <string>  imgNames1;
	// 获取对应文件夹下所有文件名
	readImgNamefromFile(fileName1, imgNames1);
	ASVLOFFSCREEN offInput1 = { 0 };
	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	ASVLOFFSCREEN offInput2 = { 0 };
	AFR_FSDK_FACEMODEL faceModels2 = { 0 };
	// 设置读入图像序列文件夹2的路径	
	std::vector <string>  imgNames2;
	// 获取对应文件夹下所有文件名
	readImgNamefromFile(fileName2, imgNames2);
	srand(time(nullptr));
	while (renum--)
	{
		//for (int in1 = 0; in1 < imgNames1.size(); ++in1)
		{
			int in1 = rand() % 30;
			offInput1 = { 0 };
			faceModels1 = { 0 };
			offInput2 = { 0 };
			faceModels2 = { 0 };
			//const char path1[] = "test_image/sfz/00王付香.bmp";
			string path1 = imgNames1[in1];
			cout << path1 << "  对应的范围是  " << 24 * in1 << "--" << 24 * (in1 + 1) << endl;
			//fout << path1 << "对应的范围是"<<24*in1<<"--"<<24*(in1+1)<<endl;
			Mat img1 = imread(path1);
			imshow("img1", img1);
			cv::waitKey(5);

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
			//for (int i = 24 * in1; i < 24 * (in1 + 1); ++i)
			//{
			//	string str = fileName2 + to_string(i + 1) + ".bmp";
			//	imgNames2.push_back(str);
			//}
			//// 遍历对应文件夹下所有文件名
			////sort(imgNames.begin(), imgNames.end());
			//for (int i = 0; i < 24; i++)
			{
				//读取第二张bmp图片数据
				//cout << imgNames[i] << endl;
				//Mat imgtemp = imread(imgNames2[i]);
				//imshow("tempimg", imgtemp);
				//waitKey(8);
				int i = rand() % 720;
				offInput2.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
				offInput2.ppu8Plane[0] = nullptr;
				readBmp(imgNames2[i].c_str(), (uint8_t**)&offInput2.ppu8Plane[0], &offInput2.i32Width, &offInput2.i32Height);
				if (!offInput2.ppu8Plane[0])
				{
					fprintf(stderr, "fail to ReadBmp(%s)\r\n", imgNames2[i]);
					free(offInput1.ppu8Plane[0]);
					AFR_FSDK_UninitialEngine(hEngine2);
					free(pWorkMem1);
					free(pWorkMem2);
					return -1;
				}
				offInput2.pi32Pitch[0] = offInput2.i32Width * 3;

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
					cout << imgNames2[i] << endl;
					fprintf(stdout, "fSimilScore =  %f\r\n", fSimilScore);

					//fout << fSimilScore << endl;24 * in1 << "--" << 24 * (in1 + 1)
					if (fSimilScore < thresh)
					{
						if (24 * in1>i || 24 * (in1 + 1) <= i)							
							fout << fSimilScore << endl;
						else
							fout << 1 << endl;
					}
					else
					{
						if (24 * in1 > i || 24 * (in1 + 1) <= i)
							//cout << imgNames2[i] << endl;
						{
							cout << "错的  " << fSimilScore << endl;
							cout << imgNames2[i] << endl;
							fouterror << path1 << "    " << imgNames2[i] << "   相似度   " << fSimilScore << endl;
							//fouterror << fSimilScore << endl;
						}
						//else
						//	cout << "对的  "<<fSimilScore << endl;
					}
				}
				else
				{
					fprintf(stderr, "FacePairMatching failed , errorcode is %d \r\n", nRet);
				}
			}
			//fout << endl;
		}
	}
	//反初始化
	fout.close();
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

