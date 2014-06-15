#ifndef __H_CAMERACONTROLLER
#define __H_CAMERACONTROLLER

#include "ofxFlyCapture2.h"
#include "ofxCv.h"
#include "cv.h"
#include <opencv2/gpu/gpu.hpp> 
#include "BaseController.h"
#include "AppModel.h"
#include "BezierWarp.h"
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

	void setColorPick(float x, float y);

	BezierWarp& getWarp();

	inline void processPixels();
	inline void processTextures();

private:

	inline void blurContour();

	BezierWarp warp;

	ofColor pick;
	Scalar offset, base;

	ofxThreadedVideo video;
	ofShader matte, blurV, blurH;
	ofFbo render0, render1;

	ofTexture contourTexture;
	ofTexture backgroundTexture;
	ofTexture thresholdTexture;
	ofTexture latencyTexture;

	vector< vector<cv::Point> > contours, tContours;
	vector<cv::Rect> boundingRects, tBoundingRects;

	cv::Mat rPixels;
	cv::Mat hPixels;
	cv::Mat tPixels;
	cv::Mat cPixels;
	cv::Mat bPixels;

	ofColor latencyPixel;
	int latencyStart, latencyTime;

	bool bBackgroundCaptured;
	bool bTestLatency;

	int erodeSize;
	int blurSize;
	int effects[2];
	float fLevel, rLevel, gLevel, bLevel;
	float viewWidth;
	float viewHeight;
	int maxArea;
	int minArea;
	bool bUseContour;
	bool bUseBackground;
	bool bInvertThreshold;
	bool bFindHoles;
	bool bApproximateMode;
	bool bUseScaleBySize;
	bool bUseSecondMonitor;
	bool bShowBackground;
	bool bUseRange;
	bool bUseWarp;
	bool bShowWarp;
	bool bUseGPU;
	float smooth;
	int totalPoints;
	int threshold;

};

#endif