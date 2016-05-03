#include "highgui.h"
#include "cgeFaceTracker.h"

using namespace CGE;

int main(int argc, const char** argv)
{
#if 0

    char ftFile[256] = "../faceTracker/model/face2.tracker";
    char conFile[256] = "../faceTracker/model/face.con";
    char triFile[256] = "../faceTracker/model/face.tri";

    CGEFaceTracker::setupTracker(ftFile, triFile, conFile);

#else

	CGEFaceTracker::setupTracker();

#endif

	if(!CGEFaceTracker::isTrackerSetup())
		return -1;

	CGEFaceTracker faceTracker;
	faceTracker.setMaxImageSize(-1); //No Image scaling
	//    faceTracker.setMaxImageSize(162); //unmark this if your device is too slow.

	CvCapture* camera = cvCreateCameraCapture(CV_CAP_ANY);
	if(!camera)
	{
		puts("You have no camera device!");
		getchar();
		return -1;
	}

	puts("Press ESC to quit.\nPress space/enter to reset face frame.");

	const std::string windowName = "Face Tracker";

	cv::namedWindow(windowName);
	cv::Mat frame,gray;

	for(;;)
	{ 
		IplImage* I = cvQueryFrame(camera);
		if(!I)
			continue;

		frame = I;

		int c = cvWaitKey(10);

		switch (c)
		{
		case 27:
			cv::destroyAllWindows();
			return 0;
		case 'd':
			faceTracker.resetFrame();
			break;
		default:
			break;
		}

		cv::cvtColor(frame,gray,CV_BGR2GRAY);

		if(faceTracker.updateFace(gray))
		{
			//            faceTracker.drawMeshes(frame, cv::Scalar(255, 0, 255, 255));
			//             faceTracker.drawFeature(frame, CGE_FACE_LEFT_EYE_TOP);
			//             faceTracker.drawFeature(frame, CGE_FACE_RIGHT_EYE_TOP);
			// 
			faceTracker.drawFeature(frame, CGE_FACE_LEFT_EYEBROW, false);
			faceTracker.drawFeature(frame, CGE_FACE_RIGHT_EYEBROW, false);
			faceTracker.drawFeature(frame, CGE_FACE_LEFT_EYE);
			faceTracker.drawFeature(frame, CGE_FACE_RIGHT_EYE);
			// 
			//             faceTracker.drawFeature(frame, CGE_FACE_LEFT_JAW, false);
			//             faceTracker.drawFeature(frame, CGE_FACE_RIGHT_JAW, false);
			faceTracker.drawFeature(frame, CGE_FACE_JAW, false);
			// 
			faceTracker.drawFeature(frame, CGE_FACE_OUTER_MOUTH);
			faceTracker.drawFeature(frame, CGE_FACE_INNER_MOUTH);
			faceTracker.drawFeature(frame, CGE_FACE_NOSE_BRIDGE, false);
			faceTracker.drawFeature(frame, CGE_FACE_NOSE_BASE);
			// 
			//             faceTracker.drawFeature(frame, CGE_FACE_OUTLINE);
			//            faceTracker.drawFeature(frame, CGE_FACE_ALL_FEATURES);
		}
		else
		{
			faceTracker.resetFrame();
		}

		imshow(windowName,frame);

	}

	return 0;
}
