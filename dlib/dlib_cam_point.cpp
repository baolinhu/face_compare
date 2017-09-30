#include <dlib/opencv.h>  
#include <opencv2/opencv.hpp>  
#include <dlib/image_processing/frontal_face_detector.h>  
#include <dlib/image_processing/render_face_detections.h>  
#include <dlib/image_processing.h>  
#include <dlib/gui_widgets.h>  

using namespace dlib;
using namespace std;


int main()
{
	try
	{
		cv::VideoCapture cap(0);
		if (!cap.isOpened())
		{
			cerr << "Unable to connect to camera" << endl;
			return 1;
		}

		//image_window win;  

		// Load face detection and pose estimation models.  
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor pose_model;
		deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

		// Grab and process frames until the main window is closed by the user.  
		while (cv::waitKey(30) != 27)
		{
			// Grab a frame  
			cv::Mat temp;
			cap >> temp;

			cv_image<bgr_pixel> cimg(temp);
			// Detect faces   
			std::vector<rectangle> faces = detector(cimg);
			// Find the pose of each face.  
			std::vector<full_object_detection> shapes;
			for (unsigned long i = 0; i < faces.size(); ++i)
				shapes.push_back(pose_model(cimg, faces[i]));

			if (!shapes.empty()) {
				for (int i = 0; i < 68; i++) {
					circle(temp, cvPoint(shapes[0].part(i).x(), shapes[0].part(i).y()), 3, cv::Scalar(0, 0, 255), -1);
					//  shapes[0].part(i).x();//68¸ö  
					//putText(temp, to_string(i), cvPoint(shapes[0].part(i).x(), shapes[0].part(i).y()), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0), 1, 4);
				}
			}
			//Display it all on the screen  
			imshow("DlibÌØÕ÷µã", temp);

		}
	}
	catch (serialization_error& e)
	{
		cout << "You need dlib's default face landmarking model file to run this example." << endl;
		cout << "You can get it from the following URL: " << endl;
		cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
		cout << endl << e.what() << endl;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}