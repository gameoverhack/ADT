#ifndef __H_CAMERACONTROLLER
#define __H_CAMERACONTROLLER

#include "ofxFlyCapture2.h"
#include "ofxCv.h"
#include "cv.h"
#include <opencv2/gpu/gpu.hpp> 
#include "BaseController.h"
#include "AppModel.h"
#include "ofxThreadedVideo.h"

using namespace ofxCv;
using namespace cv;

class CameraController : public BaseController, public ofxFlyCapture2{
    
public:
    
    CameraController();
    ~CameraController();
    
    void setup();
    void update();
	void draw();

	void setVariables();
	void setBackground();
	void setLatencyTest();

	inline void processPixels();
	inline void processTextures();

private:

	inline void blurContour();

	ofxThreadedVideo video;
	ofShader matte, blurV, blurH;
	ofFbo render0, render1;

	ofTexture contourTexture;
	ofTexture backgroundTexture;
	ofTexture latencyTexture;

	vector< vector<cv::Point> > contours, tContours;
	vector<cv::Rect> boundingRects, tBoundingRects;

	cv::Mat rPixels;
	cv::Mat tPixels;
	cv::Mat cPixels;
	cv::Mat bPixels;

	ofColor latencyPixel;
	int latencyStart, latencyTime;

	bool bBackgroundCaptured;
	bool bTestLatency;

	int erodeSize;
	int blurSize;
	float viewWidth;
	float viewHeight;
	float maxArea;
	float minArea;
	bool bUseContour;
	bool bUseBackground;
	bool bInvertThreshold;
	bool bFindHoles;
	bool bApproximateMode;
	bool bUseScaleBySize;
	bool bUseGPU;
	float smooth;
	int totalPoints;
	int threshold;

};

#endif