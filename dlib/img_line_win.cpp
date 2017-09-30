#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>  
#include<windows.h>

using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------  
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
void readImgNamefromFile(char* fileName, std::vector <string> &imgNames)
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
int main(int argc, char** argv)
{
	try
	{
		
		// 定义人脸检测器，使用它来获取一张图片中，每个人脸的边界位置  
		frontal_face_detector detector = get_frontal_face_detector();

		//定义形状预测器，用来预测给定图片和脸边界框时候的标记点的位置。  
		//这里我们从shape_predictor_68_face_landmarks.dat文件加载模型  
		shape_predictor sp;
		deserialize("shape_predictor_68_face_landmarks.dat") >> sp;
		
		//cv::Mat temp;
		//cv::namedWindow("原始图");
		//创建显示框
		image_window win, win_faces;

		char* fileName = "./images/";
		std::vector <string>  imgNames;
		// 获取对应文件夹下所有文件名
		readImgNamefromFile(fileName, imgNames);
		// 遍历对应文件夹下所有文件名
		// 循环所有图片
		for (int i = 0; i < imgNames.size(); ++i)
		{
			DWORD tin = GetTickCount();
			cout << "processing image " << imgNames[i] << endl;
			cv::Mat temp = cv::imread(imgNames[i]);
			cout << temp.cols<<temp.rows<<temp.size() << endl;

			if (temp.empty())
				cout << "图片读取错误" << endl;
			//自适应放大或缩小图片，以便准确检测和显示方便
			//这个很重要，在人脸很多的时候，每个就很小，必须要放大，才能检测到，否则检测不到
			if (temp.rows<600)
				cv::resize(temp, temp, cv::Size(), 2.0, 2.0);
			else if (temp.rows>1000)
				cv::resize(temp, temp, cv::Size(), 0.5, 0.5);

			cv_image<bgr_pixel> img(temp);

			//imshow("原始图", temp);
			//cv::waitKey(0);
			//cv::Mat temp1 = temp.clone();
			
			//array2d<rgb_pixel> img;
			//load_image(img, imgNames[i]);
			//if (img.nr()<650)
			//	pyramid_up(img);// 放大图片以便检测到比较小的人脸. 
			cout << img.nr() << "  " << img.nc() << "   "<<img.size()<<endl;

			//检测人脸，获得边界框  
			std::vector<rectangle> dets = detector(img);
			cout << "Number of faces detected: " << dets.size() << endl;

			// Now we will go ask the shape_predictor to tell us the pose of  
			// each face we detected.  
			//****调用shape_predictor类函数，返回每张人脸的姿势  
			std::vector<full_object_detection> shapes;//注意形状变量的类型，full_object_detection  
			for (unsigned long j = 0; j < dets.size(); ++j)
			{
				//预测姿势，注意输入是两个，一个是图片，另一个是从该图片检测到的边界框  
				full_object_detection shape = sp(img, dets[j]);
				cout << "number of parts: " << shape.num_parts() << endl;

				/*打印出全部68个点*/
				/*for (int i = 0; i < 68; i++)
				{
				cout << "第 " << i+1 << " 个点的坐标： " << shape.part(i) << endl;
				}*/

				shapes.push_back(shape);
			}

			//显示结果  
			win.clear_overlay();
			win.set_image(img);
			win.add_overlay(dets, rgb_pixel(255, 0, 0));
			win.add_overlay(render_face_detections(shapes));

			//我们也能提取每张对齐剪裁后的人脸的副本，旋转和缩放到一个标准尺寸  
			dlib::array<array2d<rgb_pixel> > face_chips;
			extract_image_chips(img, get_face_chip_details(shapes), face_chips);
			win_faces.set_image(tile_images(face_chips));

			cout << 1.0*(GetTickCount()-tin)/1000<<endl;
			cout << "Hit enter to process the next image..." << endl;
			cin.get();
		}
	}
	catch (exception& e)
	{
		cout << "\nexception thrown!" << endl;
		cout << e.what() << endl;
	}

	return 0;
}