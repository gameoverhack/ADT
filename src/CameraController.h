#ifndef __H_CAMERACONTROLLER
#define __H_CAMERACONTROLLER

#include "ofxFlyCapture2.h"
#include "ofxCv.h"
#include "cv.h"
#include "BaseController.h"
#include "AppModel.h"

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

	inline void processPixels();
	inline void processTextures();

private:

	vector< vector<cv::Point> > contours, tContours;
	vector<cv::Rect> boundingRects, tBoundingRects;

	cv::Mat rPixels;
	cv::Mat tPixels;
	cv::Mat cPixels;
	cv::Mat bPixels;

	ofTexture contourTex;

	float maxArea;
	float minArea;
	bool bUseContour;
	bool bUseBackground;
	bool bInvertThreshold;
	bool bFindHoles;
	bool bApproximateMode;
	bool bUseScaleBySize;
	float smooth;
	float ismooth;
	int totalPoints;
	int threshold;

};

#endif