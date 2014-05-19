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
	ofxFlyCapture2::setUseTexture(appModel->getProperty<bool>("ShowCamera"));

	rPixels.create(viewHeight, viewWidth, CV_8U);
	tPixels.create(viewHeight, viewWidth, CV_8U);
	cPixels.create(viewHeight, viewWidth, CV_8U);
	bPixels.create(viewHeight, viewWidth, CV_8U);

	contourTexture.allocate(viewWidth, viewHeight, GL_LUMINANCE);
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
		texture.draw(320 * 0, ofGetHeight() - 240, 320, 240);
		backgroundTexture.draw(320 * 1, ofGetHeight() - 240, 320, 240);
		contourTexture.draw(320 * 2, ofGetHeight() - 240, 320, 240);
		video.draw(320 * 3, ofGetHeight() - 240, 320, 240);
	}

	matte.begin();
	if(!bPixelsDirty) matte.setUniformTexture("contourTexture", (blurSize == 0 ? contourTexture : render1.getTextureReference()), 2);
	if(video.isFrameNew()) matte.setUniformTexture("videoTexture", video.getTextureReference(), 3);
	glDrawTriangleStrip(viewWidth, viewHeight);
	matte.end();
	
	if(bTestLatency) latencyTexture.draw(0, 0, viewWidth, viewHeight);

	appModel->setProperty("ProcessTime", (float)frameProcessTime);

}

//--------------------------------------------------------------
void CameraController::setVariables(){

	blurSize = appModel->getProperty<int>("Blur");
	viewWidth = appModel->getProperty<float>("OutputWidth");
	viewHeight = appModel->getProperty<float>("OutputHeight");
	maxArea = appModel->getProperty<int>("MaxContourSize") * appModel->getProperty<int>("MaxContourSize") * PI;
	minArea = appModel->getProperty<int>("MinContourSize") * appModel->getProperty<int>("MinContourSize") * PI;
	bUseContour = appModel->getProperty<bool>("UseContour");
	bUseGPU = appModel->getProperty<bool>("UseGPU");
	bInvertThreshold = appModel->getProperty<bool>("UseInvertThresh");
	bFindHoles = appModel->getProperty<bool>("UseFindHoles");
	bApproximateMode = appModel->getProperty<bool>("UseApproxMode");
	bUseScaleBySize = appModel->getProperty<bool>("UseSortBySize");
	smooth	= appModel->getProperty<float>("Smooth");
	totalPoints = appModel->getProperty<int>("Points");
	threshold = appModel->getProperty<int>("Threshold");
	bUseBackground = appModel->getProperty<bool>("UseBackground");
	
	if(erodeSize != appModel->getProperty<int>("Erode")){
		erodeSize = appModel->getProperty<int>("Erode");
		genTexCoordOffsets(viewWidth, viewHeight, erodeSize);
		matte.begin();
		matte.setUniform2fv("tcOffset", texCoordOffsets, 25);
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