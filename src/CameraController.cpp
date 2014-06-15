#include "CameraController.h"

using namespace ofxCv;
using namespace cv;

// Set up texture sampling offset storage
const GLint tcOffsetColumns = 5;
const GLint tcOffsetRows    = 5;
GLfloat	texCoordOffsets[tcOffsetColumns * tcOffsetRows * 2];
 
//--------------------------------------------------------------
void genTexCoordOffsets(GLuint width, GLuint height, GLfloat step = 1.0f){
	// Calculate texture coordinate offsets for kernel convolution effects
	// Note: You can multiply the step to displace the samples further. Do this with diff values horiz and vert and you have directional blur of a sort...
	float xInc = step / (GLfloat)(width);
	float yInc = step / (GLfloat)(height);
 
	for (int i = 0; i < tcOffsetColumns; i++)
	{
		for (int j = 0; j < tcOffsetRows; j++)
		{
			texCoordOffsets[(((i*5)+j)*2)+0] = ((-2.0f * xInc) + ((GLfloat)i * xInc)) * width;
			texCoordOffsets[(((i*5)+j)*2)+1] = ((-2.0f * yInc) + ((GLfloat)j * yInc)) * height;
		}
	}
}

//--------------------------------------------------------------
inline void glDrawTriangleStrip(float& width, float& height){
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0);	glVertex2f(0, 0);
    glTexCoord2f(0, height); glVertex2f(0, height);
    glTexCoord2f(width, 0); glVertex2f(width, 0);
    glTexCoord2f(width, height); glVertex2f(width, height);
	glEnd();
}

//--------------------------------------------------------------
struct CompareContourArea{
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
	ofxLogNotice() << "Destroying CameraController" << endl;
	ofxFlyCapture2::~ofxFlyCapture2();
}

//--------------------------------------------------------------
void CameraController::setup(){

	bool bShaderOk = false;
	bShaderOk = matte.load(ofToDataPath("effects"));
	assert(bShaderOk);
	bShaderOk = blurV.load(ofToDataPath("blur_vertical"));
	assert(bShaderOk);
	bShaderOk = blurH.load(ofToDataPath("blur_horizontal"));
	assert(bShaderOk);

	bTestLatency = bBackgroundCaptured = false;

	setVariables();
	ofxFlyCapture2::setup();
	ofxFlyCapture2::setUseOFPixels(false);
	ofxFlyCapture2::setUseTexture(appModel->getPropertyReference<bool>("ShowCamera"));

	rPixels.create(viewHeight, viewWidth, CV_8U);
	hPixels.create(viewHeight, viewWidth, CV_8U);
	tPixels.create(viewHeight, viewWidth, CV_8U);
	cPixels.create(viewHeight, viewWidth, CV_8U);
	bPixels.create(viewHeight, viewWidth, CV_8U);

	warp.allocate(viewWidth, viewHeight, 3, 3, 100);
	warp.setWarpGridPosition(100, 100, viewWidth - 200, viewHeight - 200);

	pick = ofColor(127, 127, 127);
	base = Scalar(pick.r, pick.g, pick.b);

	contourTexture.allocate(viewWidth, viewHeight, GL_LUMINANCE);
	thresholdTexture.allocate(viewWidth, viewHeight, GL_LUMINANCE);
	backgroundTexture.allocate(viewWidth, viewHeight, GL_LUMINANCE);
	latencyTexture.allocate(viewWidth, viewHeight, GL_LUMINANCE);

	cPixels.setTo(255);
	latencyTexture.loadData(cPixels.data, cPixels.cols, cPixels.rows, GL_LUMINANCE);
	pixels.allocate(viewHeight, viewWidth, OF_IMAGE_GRAYSCALE);

	render0.allocate(viewWidth, viewHeight);
	render1.allocate(viewWidth, viewHeight);
	
	if(gpu::getCudaEnabledDeviceCount() > 0){
		ofxLogNotice() << "Setting up GPU openCV" << endl;
		cv::gpu::setDevice(0);
	}
	
	setCameraFrameRate(appModel->getPropertyReference<int>("FrameRate"));
	setCameraGain(appModel->getPropertyReference<float>("Gain"));
	setCameraShutter(appModel->getPropertyReference<float>("Shutter"));

	video.setPixelFormat(OF_PIXELS_BGRA);
	video.loadMovie(ofToDataPath("C:/Users/gameoverdrive/Desktop/FlyTest.mov"));
	video.play();

	
}

//--------------------------------------------------------------
void CameraController::update(){
	ofxFlyCapture2::update();
	video.update();
}

//--------------------------------------------------------------
void CameraController::draw(){

	ofScopedLock lock(mutex);

	if(bUseTexture){
		
		backgroundTexture.draw(320 * 1, ofGetHeight() - 240, 320, 240);
		contourTexture.draw(320 * 2, ofGetHeight() - 240, 320, 240);
		video.draw(320 * 3, ofGetHeight() - 240, 320, 240);
		if(bUseSecondMonitor){
			texture.draw(0, 0, viewWidth, viewHeight);
			thresholdTexture.draw(320 * 0, ofGetHeight() - 240, 320, 240);
		}else{
			texture.draw(320 * 0, ofGetHeight() - 240, 320, 240);
		}
		
		float tW = 320 / 3.0;

		ofFill();
		ofSetColor(base[0] - offset[0], base[1] - offset[0], base[2] - offset[0]);
		ofRect(320 * 4 + tW * 0, ofGetHeight() - 240, tW, 240);

		ofSetColor(base[0], base[1], base[2]);
		ofRect(320 * 4 + tW * 1, ofGetHeight() - 240, tW, 240);

		ofSetColor(base[0] + offset[0], base[1] + offset[0], base[2] + offset[0]);
		ofRect(320 * 4 + tW * 2, ofGetHeight() - 240, tW, 240);
		ofNoFill();
		ofSetColor(255, 255, 255);
	}

	

	if(bUseWarp){
		warp.begin();
		glPushMatrix();
	}else{
		glPushMatrix();
		if(bUseSecondMonitor) glTranslatef(1920.0f, 0.0f, 0.0f);
	}

	matte.begin();
	if(!bPixelsDirty) matte.setUniformTexture("contourTexture", (blurSize == 0 ? contourTexture : render1.getTextureReference()), 2);
	if(video.isFrameNew()) matte.setUniformTexture("videoTexture", video.getTextureReference(), 3);
	glDrawTriangleStrip(viewWidth, viewHeight);
	matte.end();

	//video.draw(0, 0, 1920, 1080);

	if(bTestLatency) latencyTexture.draw(0, 0, viewWidth, viewHeight);
	if(bShowBackground) backgroundTexture.draw(0, 0, viewWidth, viewHeight);

	if(bUseWarp){
		glPopMatrix();
		warp.end();
		if(bUseSecondMonitor){
			warp.draw(viewWidth, 0, viewWidth, viewHeight);
		}else{
			warp.draw(0, 0, viewWidth, viewHeight);
		}
	}else{
		glPopMatrix();
	}

	

	appModel->setProperty("ProcessTime", (float)frameProcessTime);

}

//--------------------------------------------------------------
void CameraController::setVariables(){

	appModel->setProperty("UseVideo", true);
	appModel->setProperty("ResetWarp", false);
	appModel->setProperty("FrameRate", 50 , 1, 100);
	appModel->setProperty("Shutter", 20.0f, 1.0f, 100.0f);
	appModel->setProperty("Gain", 0.0f, 0.0f, 20.0f);
	appModel->setProperty("MinContourSize", 50, 0, 3000);
	appModel->setProperty("MaxContourSize", 2000, 0, 3000);

	appModel->setProperty("SecondMonitor", &bUseSecondMonitor);
	appModel->setProperty("ShowCamera", &bUseTexture);
	appModel->setProperty("ShowBackground", &bUseBackground);
	appModel->setProperty("UseContour", &bUseContour);
	appModel->setProperty("UseGPU", &bUseGPU);
	appModel->setProperty("UseSortBySize", &bUseGPU);
	appModel->setProperty("UseBackground", &bUseBackground);
	appModel->setProperty("UseRange", &bUseRange);
	appModel->setProperty("UseInvertThresh", &bInvertThreshold);
	appModel->setProperty("UseApproxMode", &bApproximateMode);
	appModel->setProperty("UseFindHoles", &bFindHoles);
	
	appModel->setProperty("fLevel", &fLevel, 0.0f, 1.0f);
	appModel->setProperty("rLevel", &rLevel, 0.0f, 1.0f);
	appModel->setProperty("gLevel", &gLevel, 0.0f, 1.0f);
	appModel->setProperty("bLevel", &bLevel, 0.0f, 1.0f);
	appModel->setProperty("Threshold", &threshold, 0, 255);
	appModel->setProperty("Smooth", &smooth, 0.0f, 1.0f);
	appModel->setProperty("Erode", &erodeSize, 0, 100);
	appModel->setProperty("Blur", &blurSize, 0, 10);

	appModel->setProperty("ShowWarp", &bShowWarp);
	appModel->setProperty("UseWarp", &bUseWarp);

	//appModel->setProperty("ShowCamera", false);
	//appModel->setProperty("ShowBackground", false);
	//appModel->setProperty("UseContour", true);
	//appModel->setProperty("UseGPU", false);
	//appModel->setProperty("UseSortBySize", false);
	//appModel->setProperty("UseBackground", false);
	//appModel->setProperty("UseRange", false);
	//appModel->setProperty("UseInvertThresh", false);
	//appModel->setProperty("UseApproxMode", true);
	//appModel->setProperty("UseFindHoles", false);
	//appModel->setProperty("UseVideo", true);
	//appModel->setProperty("fLevel", 1.0f, 0.0f, 1.0f);
	//appModel->setProperty("rLevel", 1.0f, 0.0f, 1.0f);
	//appModel->setProperty("gLevel", 1.0f, 0.0f, 1.0f);
	//appModel->setProperty("bLevel", 1.0f, 0.0f, 1.0f);
	//appModel->setProperty("Threshold", 100, 0, 255);
	//appModel->setProperty("Smooth", 0.5f, 0.0f, 1.0f);
	//appModel->setProperty("Erode", 0, 0, 100);
	//appModel->setProperty("Blur", 0, 0, 10);
	//appModel->setProperty("MinContourSize", 50, 0, 4000);
	//appModel->setProperty("MaxContourSize", 2000, 0, 4000);
	//appModel->setProperty("ShowWarp", false);
	//appModel->setProperty("ResetWarp", false);
	//appModel->setProperty("UseWarp", true);
	//appModel->setProperty("FrameRate", 50 , 1, 100);
	//appModel->setProperty("Shutter", 20.0f, 1.0f, 100.0f);
	//appModel->setProperty("Gain", 0.0f, 0.0f, 20.0f);

	blurSize = 0;
	viewWidth = appModel->getPropertyReference<float>("OutputWidth");
	viewHeight = appModel->getPropertyReference<float>("OutputHeight");
	maxArea = appModel->getPropertyReference<int>("MaxContourSize") * appModel->getPropertyReference<int>("MaxContourSize") * PI;
	minArea = appModel->getPropertyReference<int>("MinContourSize") * appModel->getPropertyReference<int>("MinContourSize") * PI;
	bUseContour = true;
	bUseGPU = false;
	bInvertThreshold = false;
	bFindHoles = false;
	bApproximateMode = true;
	bUseScaleBySize = false;
	smooth = 0.5f;
	threshold = 100;
	bUseBackground = false;
	bUseSecondMonitor = true;
	bShowBackground = false;
	bUseWarp = true;
	bShowWarp = false;
	bUseRange = false;

	//setUseTexture(appModel->getPropertyReference<bool>("ShowCamera"));
	warp.setShowWarpGrid(bShowWarp);
	offset = Scalar(threshold, threshold, threshold);

	if(erodeSize != appModel->getPropertyReference<int>("Erode")){

		erodeSize = appModel->getPropertyReference<int>("Erode");
		genTexCoordOffsets(viewWidth, viewHeight, erodeSize);

		matte.begin();
		matte.setUniform2fv("tcOffset", texCoordOffsets, 25);
		matte.end();

	}
	
	if(effects[0] != (int)(appModel->getPropertyReference<bool>("UseVideo")) ||
		fLevel != appModel->getPropertyReference<float>("fLevel") ||
		rLevel != appModel->getPropertyReference<float>("rLevel") ||
		gLevel != appModel->getPropertyReference<float>("gLevel") ||
		bLevel != appModel->getPropertyReference<float>("bLevel")){

		fLevel = appModel->getPropertyReference<float>("fLevel");
		rLevel = appModel->getPropertyReference<float>("rLevel");
		gLevel = appModel->getPropertyReference<float>("gLevel");
		bLevel = appModel->getPropertyReference<float>("bLevel");

		effects[0] = (int)(appModel->getPropertyReference<bool>("UseVideo"));
		
		matte.begin();
		matte.setUniform1iv("effects", effects, 2);
		matte.setUniform1f("fLevel", fLevel);
		matte.setUniform1f("rLevel", rLevel);
		matte.setUniform1f("bLevel", bLevel);
		matte.setUniform1f("gLevel", gLevel);
		matte.end();
	}
}

//--------------------------------------------------------------
void CameraController::setBackground(){
	ofScopedLock lock(mutex);
	ofLogNotice() << "Capturing background" << endl;
	cPixels.data = rawImage.GetData();
	cPixels.copyTo(bPixels);
	backgroundTexture.loadData(bPixels.data, bPixels.cols, bPixels.rows, GL_LUMINANCE);
	bBackgroundCaptured = true;
}

//--------------------------------------------------------------
void CameraController::setLatencyTest(){
	ofScopedLock lock(mutex);
	latencyStart = ofGetElapsedTimeMillis();
	pixels.setFromPixels(rawImage.GetData(), rawImage.GetCols(), rawImage.GetRows(), OF_IMAGE_GRAYSCALE);
	latencyPixel = pixels.getColor(viewWidth / 2, viewHeight / 2);
	bTestLatency = true;
}

//--------------------------------------------------------------
void CameraController::setColorPick(float x, float y){
	ofScopedLock lock(mutex);
	pixels.setFromPixels(rawImage.GetData(), rawImage.GetCols(), rawImage.GetRows(), OF_IMAGE_GRAYSCALE);
	pick = pixels.getColor(x, y);
	base = Scalar(pick.r, pick.g, pick.b);
}

//--------------------------------------------------------------
BezierWarp& CameraController::getWarp(){
	return warp;
}

//--------------------------------------------------------------
inline void CameraController::processPixels(){

	ofxFlyCapture2::processPixels();

	if(bTestLatency){
			
		pixels.setFromPixels(rawImage.GetData(), rawImage.GetCols(), rawImage.GetRows(), OF_IMAGE_GRAYSCALE);
		//cout << "t: " << pixels.getColor(viewWidth / 2, viewHeight / 2) << "  >>>  " << latencyPixel << "   " << int(viewWidth * (viewHeight / 2) + viewHeight / 2) << endl;
		if(pixels.getColor(viewWidth / 2, viewHeight / 2).r == 255){
			latencyTime = ofGetElapsedTimeMillis() - latencyStart;
			appModel->setProperty("Latency", latencyTime);
			bTestLatency = false;
		}
	}

	if(bUseContour){

		cPixels.data = tempImage.GetData();
		cPixels.copyTo(rPixels);

		unlock();

		if(bUseBackground && bBackgroundCaptured){
			cv::absdiff(rPixels, bPixels, rPixels);
		}

		if(bUseRange){
			cv::inRange(rPixels, base - offset, base + offset, tPixels);
		}else{
			int thresholdType = bInvertThreshold ? THRESH_BINARY_INV : THRESH_BINARY;
			float maxVal = numeric_limits<uint8_t>::max();
			cv::threshold(rPixels, tPixels, threshold, maxVal, thresholdType);
		}
		
		if(bUseTexture) tPixels.copyTo(hPixels);

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
		//tBoundingRects.clear();

		for(int i = 0; i < allIndices.size(); i++) {

			totalPoints = ceil( (float)allContours[allIndices[i]].size() * smooth);
			for(int k = 0; k < totalPoints; k++){ // change to 1 if using smooth
				int j = ceil( ( (float)allContours[allIndices[i]].size() / (float)totalPoints ) * k );
				tContours[i].push_back(allContours[allIndices[i]][j]);	
			}
			
			//tBoundingRects.push_back(boundingRect(tContours[i]));

		} //for(int i = 0; i < allIndices.size(); i++) {

		//tracker.trackContour(tBoundingRects, tContours);

		lock();
		//boundingRects = tBoundingRects;
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

	contourTexture.loadData(tPixels.data, tPixels.cols, tPixels.rows, GL_LUMINANCE);
	if(bUseTexture) thresholdTexture.loadData(hPixels.data, hPixels.cols, hPixels.rows, GL_LUMINANCE);
	if(blurSize > 0) blurContour();
}

//--------------------------------------------------------------
inline void CameraController::blurContour(){

	render1.begin();
	ofClear(0,0,0,1);
	render1.end();

	for (int pass = 0; pass < pow((double)blurSize, 2); pass++) {
		
		render0.begin();
		{
			ofClear(0,0,0,1);
			blurV.begin();
			{
				blurV.setUniformTexture("texture", (pass == 0 ? contourTexture : render1.getTextureReference()), 1);
				blurV.setUniform1f("blurSize", (float)blurSize);
				glDrawTriangleStrip(viewWidth, viewHeight);
			}
			blurV.end();
		}
		
		render0.end();
		
		render1.begin();
		{
			blurH.begin();
			{
				blurH.setUniformTexture("texture", render0.getTextureReference(), 1);
				blurH.setUniform1f("blurSize", (float)blurSize);
				glDrawTriangleStrip(viewWidth, viewHeight);
			}
			
			blurH.end();
		}
		render1.end();

	} // for (int pass = 0; pass < pow((double)blurSize, 2); pass++) {
}