#define SHOW_CONSOLE

#include "graphics.h"
#include "highgui.h"
#include "cgeFaceTracker.h"

#include <windows.h>

#include <vector>

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

class MySprite
{
public:
	MySprite() : m_index(0) {}

	~MySprite()
	{
		for(PIMAGE pimage : m_sprites)
		{
			delimage(pimage);
		}

		m_sprites.clear();
	}

	bool init(const char* pattern, int count)
	{
		m_sprites.reserve(100);
		char imageFileName[128];
		for(int i = 0; i != count; ++i)
		{
			sprintf(imageFileName, pattern, i);
			PIMAGE image = newimage();
			int ret = getimage_pngfile(image, imageFileName);
			if(ret != 0)
				return false;
			m_sprites.push_back(image);
		}

		PIMAGE img = m_sprites[0];

		m_width = getwidth(img);
		m_height = getheight(img);

		return true;
	}

	void render(int x, int y, float centerX, float centerY, float rot, float scaling)
	{
		putimage_rotatezoom(nullptr, m_sprites[m_index], x, y, centerX, centerY, rot, scaling, 1);
		//putimage(x, y, m_sprites[m_index]);
		++m_index;
		m_index %= m_sprites.size();
	}

	inline float getWidth() { return m_width; }
	inline float getHeight() { return m_height; }

private:
	std::vector<PIMAGE> m_sprites;
	float m_width, m_height;
	int m_index;
};


int main()
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
	//faceTracker.setMaxImageSize(-1); //No Image scaling -- unmark this if you need more accuracy.
	faceTracker.setMaxImageSize(240); //Scale image by 240 -- unmark this if your device is too slow.

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
	setbkmode(TRANSPARENT);
	setcolor(0x00ff0000);

	setcaption("EGE FaceTracker By wysaid.");

	cv::Mat frame,gray;
	PIMAGE egeImage = newimage(scrWidth, scrHeight);
	cv::Mat egeMat(getheight(egeImage), getwidth(egeImage), CV_8UC4, getbuffer(egeImage));

	MySprite catSprite, glassSprite;

	if(!(catSprite.init("../res/catjump_%03d.png", 100) && glassSprite.init("../res/glasses_%03d.png", 66)))
	{
		LOG_ERROR("Init sprite failed!");
		return -1;
	}

	for(; is_run(); delay_fps(30))
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
				cvReleaseCapture(&camera);
				return 0;
			default:
				faceTracker.resetFrame();
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

		bool hasFace = faceTracker.updateFace(gray);

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
			cv::cvtColor(frame, egeMat,CV_BGR5652BGRA);
			break;
		case CV_8UC3:
			cv::cvtColor(frame, egeMat,CV_BGR2BGRA); //Almost
			break;
		case CV_8UC4:
			frame.copyTo(egeMat);
			break;
		default:
			LOG_ERROR("unsupported video format!");
			return -1;
		}

		putimage(0, 0, egeImage);
		if(hasFace)
		{
			const Vec2f& v = faceTracker.getEyeCenterPos();
			Vec2f&& eyeDir = faceTracker.getRightDir();
			float eyeDis = eyeDir.length();
			float roll = -asinf(eyeDir[1] / eyeDis);
			if(eyeDir[0] < 0.0f)
			{
				roll = PI - roll;
			}
			else
			{
				roll += PI * 2.0;
			}

			float catScaling = eyeDis / catSprite.getWidth() * 4.0f;
			catSprite.render(v[0], v[1], 0.5f, 1.0f, roll, catScaling);
			float glassScaling = eyeDis / glassSprite.getWidth() * 2.5f;
			glassSprite.render(v[0], v[1], 0.5f, 0.5f, roll, glassScaling);
		}
		else
		{
			faceTracker.resetFrame();
		}

		outtextxy(10, 10, "EGE Face Tracker - Single Thread Demo.");
		outtextxy(10, 50, "Press esc to exit or the others to reset face mesh.");

		{
			static clock_t lastTime = clock();
			static int frameCount = 0;
			++frameCount;
			clock_t currentTime = clock();
			if (currentTime - lastTime > CLOCKS_PER_SEC)
			{
				LOG_INFO("Ïà»úÖ¡ÂÊ: %d\n", frameCount);
				frameCount = 0;
				lastTime = currentTime;
			}
		}
	}

	return 0;
}
