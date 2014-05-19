#include "CameraController.h"

using namespace ofxCv;
using namespace cv;

// Set up texture sampling offset storage
const GLint tcOffsetColumns = 5;
const GLint tcOffsetRows    = 5;
GLfloat	texCoordOffsets[tcOffsetColumns * tcOffsetRows * 2];
 
// Calculate texture coordinate offsets for kernel convolution effects
void genTexCoordOffsets(GLuint width, GLuint height, GLfloat step = 1.0f) // Note: Change this step value to increase the number of pixels we sample across...
{
	// Note: You can multiply the step to displace the samples further. Do this with diff values horiz and vert and you have directional blur of a sort...
	float xInc = step / (GLfloat)(width);
	float yInc = step / (GLfloat)(height);
 
	for (int i = 0; i < tcOffsetColumns; i++)
	{
		for (int j = 0; j < tcOffsetRows; j++)
		{
			texCoordOffsets[(((i*5)+j)*2)+0] = (-2.0f * xInc) + ((GLfloat)i * xInc);
			texCoordOffsets[(((i*5)+j)*2)+1] = (-2.0f * yInc) + ((GLfloat)j * yInc);
		}
	}
}

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
	contourTexture.allocate(viewWidth, viewHeight, GL_LUMINANCE);
	videoTexture.allocate(viewWidth, viewHeight, GL_BGRA);
	render0.allocate(viewWidth, viewHeight);
	render1.allocate(viewWidth, viewHeight);
	genTexCoordOffsets(viewWidth, viewHeight);

	if(gpu::getCudaEnabledDeviceCount() > 0){
		ofxLogNotice() << "Setting up GPU openCV" << endl;
		cv::gpu::setDevice(0);
	}
	
	video.setPixelFormat(OF_PIXELS_BGRA);
	video.loadMovie(ofToDataPath("C:/Users/gameoverdrive/Desktop/FlyTest.mov"));
	video.play();

	bool bShaderOk = false;
	bShaderOk = matte.load(ofToDataPath("matte"));
	assert(bShaderOk);
	bShaderOk = blurV.load(ofToDataPath("blur_vertical"));
	assert(bShaderOk);
	bShaderOk = blurH.load(ofToDataPath("blur_horizontal"));
	assert(bShaderOk);
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
		texture.draw(240 * 0, ofGetHeight() - 240, 320, 240);
		contourTexture.draw(240 * 1, ofGetHeight() - 240, 320, 240);
		video.draw(240 * 2, ofGetHeight() - 240, 320, 240);
	}
	
	matte.begin();
	if(!bPixelsDirty) matte.setUniformTexture("contourTexture", (blurSize == 0 ? contourTexture : render1.getTextureReference()), 2);
	if(video.isFrameNew()) matte.setUniformTexture("videoTexture", video.getTextureReference(), 3);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0);	glVertex2f(0, 0);
    glTexCoord2f(0, viewHeight); glVertex2f(0, viewHeight);
    glTexCoord2f(viewWidth, 0); glVertex2f(viewWidth, 0);
    glTexCoord2f(viewWidth, viewHeight); glVertex2f(viewWidth, viewHeight);
	glEnd();
	matte.end();
	
	if(ofGetFrameNum() % 25 == 0) appModel->setProperty("ProcessTime", (float)frameProcessTime);
}

void CameraController::setVariables(){
	erodeSize = appModel->getProperty<float>("Erode");
	blurSize = appModel->getProperty<int>("Blur");
	viewWidth = appModel->getProperty<float>("OutputWidth");
	viewHeight = appModel->getProperty<float>("OutputHeight");
	maxArea = appModel->getProperty<int>("MaxContourSize") * appModel->getProperty<int>("MaxContourSize") * PI;
	minArea = appModel->getProperty<int>("MinContourSize") * appModel->getProperty<int>("MinContourSize") * PI;
	bUseContour = appModel->getProperty<bool>("UseContour");
	bUseGPU = appModel->getProperty<bool>("UseGPU");
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
			if(gpu::getCudaEnabledDeviceCount() > 0){
				gSPixels.create(rawImage.GetRows(), rawImage.GetCols(), CV_8U);
				gDPixels.create(rawImage.GetRows(), rawImage.GetCols(), CV_8U);
			}
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

			for(int k = 0; k < totalPoints; k++){ // change to 1 if using smooth
				int j = ceil( ( (float)allContours[allIndices[i]].size() / (float)totalPoints ) * k );
				//tContours[i].push_back(allContours[allIndices[i]][j] * smooth + allContours[allIndices[i]][j - 1] * ismooth);	
				tContours[i].push_back(allContours[allIndices[i]][j]);	
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

		//cv::erode(tPixels, tPixels, Mat(), cv::Point(-1, -1), ceil(erodeSize));

		//if(bUseGPU){
		//	gSPixels.upload(tPixels);
		//	cv::gpu::erode(gSPixels, gDPixels, Mat(), cv::Point(-1, -1), 2);
		//	cv::gpu::blur(gDPixels, gSPixels, cv::Size2i(10,10));
		//	gSPixels.download(tPixels);
		//}else{
		//	cv::erode(tPixels, tPixels, Mat(), cv::Point(-1, -1), 2);
		//	cv::blur(tPixels, tPixels, cv::Size2i(10,10));
		//}
		

	} //if(appModel->getProperty<bool>("UseContour"){

}

//--------------------------------------------------------------
inline void CameraController::processTextures(){

	if(contourTexture.getWidth() != tPixels.cols || contourTexture.getHeight() != tPixels.rows){
		contourTexture.allocate(tPixels.cols, tPixels.rows, GL_LUMINANCE);
	}

	contourTexture.loadData(tPixels.data, tPixels.cols, tPixels.rows, GL_LUMINANCE);

	//render1.begin();
	//ofClear(0,0,0,1);
	//float xErode = tPixels.cols * erodeSize / 2.0f;
	//float yErode = tPixels.rows * erodeSize / 2.0f;
	//contourTexture.draw(0, 0, tPixels.cols - xErode, tPixels.rows - yErode);
	//render1.end();

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
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0, 0);	glVertex2f(0, 0);
				glTexCoord2f(0, viewHeight); glVertex2f(0, viewHeight);
				glTexCoord2f(viewWidth, 0); glVertex2f(viewWidth, 0);
				glTexCoord2f(viewWidth, viewHeight); glVertex2f(viewWidth, viewHeight);
				glEnd();
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
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0, 0);	glVertex2f(0, 0);
				glTexCoord2f(0, viewHeight); glVertex2f(0, viewHeight);
				glTexCoord2f(viewWidth, 0); glVertex2f(viewWidth, 0);
				glTexCoord2f(viewWidth, viewHeight); glVertex2f(viewWidth, viewHeight);
				glEnd();
			}
			

			blurH.end();
		}
		render1.end();

	} // for (int pass = 0; pass < pow((double)blurSize, 2); pass++) {
}