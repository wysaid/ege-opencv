#define SHOW_CONSOLE

#include "graphics.h"
#include "highgui.h"
#include "cgeFaceTracker.h"

#include <windows.h>

#define F_RED		FOREGROUND_INTENSITY | FOREGROUND_RED
#define F_BLUE		FOREGROUND_INTENSITY | FOREGROUND_BLUE
#define F_GREEN		FOREGROUND_INTENSITY | FOREGROUND_GREEN
#define F_YELLOW	FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN
#define F_PURPLE	FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE
#define F_CYAN		FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE

#define B_RED		BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED
#define B_BLUE		BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_BLUE
#define B_GREEN		BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_GREEN
#define B_YELLOW	BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN
#define B_PURPLE	BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE
#define B_CYAN		BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE

#define LOG_INFO(...) \
do{\
SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
printf(__VA_ARGS__);\
fflush(stdout);\
}while(0)

#define LOG_ERROR(...) \
do{\
SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), F_YELLOW | B_RED);\
fprintf(stderr, __VA_ARGS__);\
fflush(stderr);\
}while(0)

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
    {
        LOG_ERROR("Setup tracker failed!");
        getchar();
        return -1;
    }

	CGEFaceTracker faceTracker;
	faceTracker.setMaxImageSize(-1); //No Image scaling -- unmark this if you need more accuracy.
	//faceTracker.setMaxImageSize(162); //Scale image by 162 -- unmark this if your device is too slow.

	CvCapture* camera = cvCreateCameraCapture(CV_CAP_ANY);
	if(!camera)
	{
		LOG_ERROR("Your device has no camera!! This demo needs a camera.\n");
		getchar();
		return -1;
	}

	LOG_INFO("Press ESC to quit.\nPress space/enter to reset face frame.\n");

    int scrWidth = 640, scrHeight = 480;

    setinitmode(INIT_RENDERMANUAL);
    initgraph(scrWidth, scrHeight);

    setcaption("EGE FaceTracker By wysaid.");

	cv::Mat frame,gray;
    PIMAGE egeImage = newimage(scrWidth, scrHeight);
    cv::Mat egeMat(getheight(egeImage), getwidth(egeImage), CV_8UC4, getbuffer(egeImage));

	for(; is_run(); delay_fps(60))
	{ 
		IplImage* I = cvQueryFrame(camera);
		if(I == nullptr)
        {
            LOG_INFO("Info: no capture for current loop, continue...");
            continue;
        }

		frame = I;

        if(kbhit())
        {
            key_msg c = getkey();

            switch (c.key)
            {
            case 27:
                return 0;
            case 'd':
                faceTracker.resetFrame();
                break;
            default:
                break;
            }
        }

        switch (frame.type())
        {
        case CV_8UC1:
            gray = frame;
            break;
        case CV_8UC2:
            cv::cvtColor(frame,gray,CV_BGR5652GRAY);
            break;
        case CV_8UC3:
            cv::cvtColor(frame,gray,CV_BGR2GRAY); //Almost
            break;
        case CV_8UC4:
            cv::cvtColor(frame,gray,CV_BGRA2GRAY);
            break;
        default:
            LOG_ERROR("unsupported video format!");
            return -1;
        }

		if(faceTracker.updateFace(gray, -1, 5, 3.0, 0.0001, false))
		{
//          faceTracker.drawMeshes(frame, cv::Scalar(255, 0, 255, 255));
			faceTracker.drawFeature(frame, CGE_FACE_LEFT_EYEBROW, false);
			faceTracker.drawFeature(frame, CGE_FACE_RIGHT_EYEBROW, false);
			faceTracker.drawFeature(frame, CGE_FACE_LEFT_EYE);
			faceTracker.drawFeature(frame, CGE_FACE_RIGHT_EYE);
			faceTracker.drawFeature(frame, CGE_FACE_JAW, false);
			faceTracker.drawFeature(frame, CGE_FACE_OUTER_MOUTH);
			faceTracker.drawFeature(frame, CGE_FACE_INNER_MOUTH);
			faceTracker.drawFeature(frame, CGE_FACE_NOSE_BRIDGE, false);
			faceTracker.drawFeature(frame, CGE_FACE_NOSE_BASE);
		}
		else
		{
			faceTracker.resetFrame();
		}

		if(frame.rows != scrHeight || frame.cols != scrWidth)
        {
            scrWidth = frame.cols;
            scrHeight = frame.rows;
            initgraph(scrWidth, scrHeight);
            LOG_INFO("reset window size: %dx%d", scrWidth, scrHeight);
            delimage(egeImage);
            egeImage = newimage(scrWidth, scrHeight);
            egeMat = cv::Mat(scrHeight, scrWidth, CV_8UC4, getbuffer(egeImage));
        }

        switch (frame.type())
        {
        case CV_8UC1:
            cv::cvtColor(frame, egeMat, CV_GRAY2BGRA);
            break;
        case CV_8UC2:
            cv::cvtColor(frame,egeMat,CV_BGR5652BGRA);
            break;
        case CV_8UC3:
            cv::cvtColor(frame,egeMat,CV_BGR2BGRA); //Almost
            break;
        case CV_8UC4:
            frame.copyTo(egeMat);
            break;
        default:
            LOG_ERROR("unsupported video format!");
            return -1;
        }
        
        cv::line(egeMat, cv::Point(200, 200), cv::Point(400, 400), cv::Scalar(255, 0, 0, 255), 3);

        putimage(0, 0, egeImage);
        line(0, 0, 100, 100);
	}

	return 0;
}
