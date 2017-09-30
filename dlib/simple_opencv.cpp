#include <dlib/opencv.h>  
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <dlib/image_processing/frontal_face_detector.h>  
#include <dlib/image_processing/render_face_detections.h>  
#include <dlib/image_processing.h>  
#include <dlib/gui_widgets.h>  

using namespace std;
using namespace cv;
using namespace dlib;

#pragma comment(lib,"dlib.lib")  

int main()
{
	
	char img_file[] = "./test/2007_007763.jpg";
	char landmark_file[] = "shape_predictor_68_face_landmarks.dat";

	//  
	cv::Mat temp = cv::imread(img_file);
	
	if (temp.empty())
		cout << "图片读取错误" << endl;
	
	//自适应放大或缩小图片，以便准确检测和显示方便
	//这个很重要，在人脸很多的时候，每个就很小，必须要放大，才能检测到，否则检测不到
	if (temp.rows<600)
		cv::resize(temp, temp, cv::Size(), 2.0, 2.0);
	else 
	{
		if (temp.rows>1000)
			cv::resize(temp, temp, cv::Size(), 0.5, 0.5);
	}
	//  
	frontal_face_detector detector = get_frontal_face_detector();

	shape_predictor sp;
	deserialize(landmark_file) >> sp;

	cv_image<dlib::bgr_pixel> arrImg(temp);
	//load_image(arrImg, img_file);
	
	std::vector<dlib::rectangle> dets = detector(arrImg);
	for (unsigned long j = 0; j < dets.size(); ++j)
	{
		full_object_detection shape = sp(arrImg, dets[j]);
		cv::rectangle(temp, cv::Point(dets[j].left(),dets[j].top()),cv::Point(dets[j].right(),dets[j].bottom()), Scalar(255, 0, 0), 2, 8, 0);
		for (unsigned long i = 0; i<shape.num_parts(); i++)
		{
			point pt = shape.part(i);
			int x = pt.x();
			int y = pt.y();
			circle(temp, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1);
			//line(img, cv::Point(pt.x(), pt.y()), cv::Point(pt.x(), pt.y()), cv::Scalar(0, 0, 255), 5);
		}
	}
	//保存图片
	//cv::imwrite("./ret/3i.jpg", img); 
	cv::namedWindow("img", 0);
	imshow("img", temp);
	cv::waitKey();
	return 0;
}