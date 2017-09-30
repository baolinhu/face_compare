#include <iostream>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <fstream>

using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
	try
	{
		frontal_face_detector detector = get_frontal_face_detector();//定义一个frontal_face_detctor类的实例detector，用get_frontal_face_detector函数初始化该实例
		image_window win;//一个显示窗口

		array2d<unsigned char> img;
		load_image(img, "./test/2007_007763.jpg");// 加载一张图片，从argv[i](图片路劲)加载到变量img

		pyramid_up(img);//对图像进行上采用，检测更小的人脸

		// Now tell the face detector to give us a list of bounding boxes
		// around all the faces it can find in the image.
		//开始检测，返回一系列的边界框
		std::vector<rectangle> dets = detector(img);//detector（）函数检测人脸，返回一系列边界盒子

		cout << "Number of faces detected: " << dets.size() << endl;//dets.size 人脸数量
		// 在原图片上显示结果
		win.clear_overlay();
		win.set_image(img);
		win.add_overlay(dets, rgb_pixel(255, 0, 0));

		cout << "Hit enter to process the next image..." << endl;
		cin.get();
	}
	catch (exception& e)
	{
		cout << "\nexception thrown!" << endl;
		cout << e.what() << endl;
	}
}