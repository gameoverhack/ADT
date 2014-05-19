#include "CameraController.h"

using namespace ofxCv;
using namespace cv;

struct CompareContourArea
{
	CompareContourArea(const std::vector<double>& areaVec)
	: mAreaVec(areaVec) {}

	// Sort contour indices into decreasing order, based on a vector of
	// contour areas.  Later, we will use these indices to order the
	// contours (which are stored in a separate vector).
	bool operator()(size_t a, size_t b) const
	{
		return mAreaVec[a] > mAreaVec[b];
	}

	const std::vector<double>& mAreaVec;
};

//--------------------------------------------------------------
CameraController::CameraController(){
	ofxLogNotice() << "Creating CameraController" << endl;
}

//--------------------------------------------------------------
CameraController::~CameraController(){
	ofxLogNotice() << "Creating CameraController" << endl;
}

//--------------------------------------------------------------
void CameraController::setup(){
	setVariables();
	ofxFlyCapture2::setup();
	ofxFlyCapture2::setUseOFPixels(false);
	ofxFlyCapture2::setUseTexture(appModel->getProperty<bool>("ShowCamera"));
	contourTex.allocate(ofxFlyCapture2::getWidth(), ofxFlyCapture2::getHeight(), GL_LUMINANCE);
}

//--------------------------------------------------------------
void CameraController::update(){
	ofxFlyCapture2::update();
}

//--------------------------------------------------------------
void CameraController::draw(){
	//ofxFlyCapture2::draw();
	ofScopedLock lock(mutex);
	if(bUseTexture) texture.draw(0, 0);
	contourTex.draw(0, ofGetHeight() - 480, 640, 480);
	if(ofGetFrameNum() % 1000 == 0) appModel->setProperty("ProcessTime", (float)frameProcessTime);
}

void CameraController::setVariables(){
	maxArea = appModel->getProperty<int>("MaxContourSize") * appModel->getProperty<int>("MaxContourSize") * PI;
	minArea = appModel->getProperty<int>("MinContourSize") * appModel->getProperty<int>("MinContourSize") * PI;
	bUseContour = appModel->getProperty<bool>("UseContour");
	bUseBackground = appModel->getProperty<bool>("UseBackground");
	bInvertThreshold = appModel->getProperty<bool>("UseInvertThresh");
	bFindHoles = appModel->getProperty<bool>("UseFindHoles");
	bApproximateMode = appModel->getProperty<bool>("UseApproxMode");
	bUseScaleBySize = appModel->getProperty<bool>("UseSortBySize");
	smooth	= appModel->getProperty<float>("Smooth");
	ismooth	= 1.0f - smooth;
	totalPoints = appModel->getProperty<int>("Points");
	threshold = appModel->getProperty<int>("Threshold");
}

//--------------------------------------------------------------
inline void CameraController::processPixels(){
	ofxFlyCapture2::processPixels();

	if(bUseContour){

		if(rPixels.cols == 0 && rPixels.rows == 0){
			rPixels.create(rawImage.GetRows(), rawImage.GetCols(), CV_8U);
			tPixels.create(rawImage.GetRows(), rawImage.GetCols(), CV_8U);
			cPixels.create(rawImage.GetRows(), rawImage.GetCols(), CV_8U);
			bPixels.create(rawImage.GetRows(), rawImage.GetCols(), CV_8U);
		} //if(rPixels.cols == 0 && rPixels.rows == 0){

		cPixels.data = tempImage.GetData();
		cPixels.copyTo(rPixels);

		unlock();

		if(bUseBackground){
			cv::absdiff(rPixels, bPixels, rPixels);
		}

		int thresholdType = bInvertThreshold ? THRESH_BINARY_INV : THRESH_BINARY;
		float maxVal = numeric_limits<uint8_t>::max();
		
		cv::threshold(rPixels, tPixels, threshold, maxVal, thresholdType);
		
		CvContourRetrievalMode contourFindingMode = (bFindHoles) ? CV_RETR_LIST : CV_RETR_EXTERNAL;
		int simplifyMode = bApproximateMode ? CV_CHAIN_APPROX_SIMPLE : CV_CHAIN_APPROX_NONE;

		vector<vector<cv::Point> > allContours;
		cv::findContours(tPixels, allContours, contourFindingMode, simplifyMode);

		vector<size_t> allIndices;
		vector<double> allAreas;	
			
		for(size_t i = 0; i < allContours.size(); i++){
			double area = contourArea(allContours[i]);
			allAreas.push_back(area);
			if(area >= minArea && area <= maxArea){
				allIndices.push_back(i);
			}
		}

		// Sort contour indices, based on a separate vector of areas.
		if (allIndices.size() > 1 && bUseScaleBySize) {
			std::sort(allIndices.begin(), allIndices.end(), CompareContourArea(allAreas));
		}

		tContours.clear();
		tContours.resize(allIndices.size());
		tBoundingRects.clear();

		for(int i = 0; i < allIndices.size(); i++) {

			for(int k = 1; k < totalPoints; k++){
				int j = ceil( ( (float)allContours[allIndices[i]].size() / (float)totalPoints ) * k );
				tContours[i].push_back(allContours[allIndices[i]][j] * smooth + allContours[allIndices[i]][j - 1] * ismooth);	
			}
			
			tBoundingRects.push_back(boundingRect(tContours[i]));

		} //for(int i = 0; i < allIndices.size(); i++) {

		//tracker.trackContour(tBoundingRects, tContours);

		lock();
		boundingRects = tBoundingRects;
		contours = tContours;
		//smoothedContour = tracker.smoothedContour;
		//labels = tracker.getCurrentLabels();
		tPixels.setTo(0);
		Scalar color(255, 255, 255);
		drawContours(tPixels, contours, -1, color, CV_FILLED);

	} //if(appModel->getProperty<bool>("UseContour"){

}

//--------------------------------------------------------------
inline void CameraController::processTextures(){
	if(contourTex.getWidth() != tPixels.cols || contourTex.getHeight() != tPixels.rows){
		contourTex.allocate(tPixels.cols, tPixels.rows, GL_LUMINANCE);
	}
	contourTex.loadData(tPixels.data, tPixels.cols, tPixels.rows, GL_LUMINANCE);
}