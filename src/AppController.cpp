//
//  AppController.cpp
//  LAFTest
//
//  Created by gameover on 9/01/14.
//
//

#include "AppController.h"

class Cunt{
public:
	Cunt(){
			
		for(int i = 0; i < 1000; i++){
			somecrap.push_back(ofRandom(1000));
		}
	}

	vector<int> somecrap;
	

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){
        ar & BOOST_SERIALIZATION_NVP(somecrap);

	}
};

//--------------------------------------------------------------
AppController::AppController(){
    ofxLogNotice() << "Creating AppController" << endl;
}

//--------------------------------------------------------------
AppController::~AppController(){
    ofxLogNotice() << "Destroying AppController" << endl;
}

//--------------------------------------------------------------
void AppController::setup(){
    
    // set log levels
    ofxLogSetLogLevel(LOG_VERBOSE);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
#if(OF_VERSION_MINOR == 8)
    ofSetLogLevel("ofThread", OF_LOG_NOTICE);
#endif
    
    ofxLogNotice() << "AppController start setup" << endl;
    
    /******************************************************
     *******                States                  *******
     *****************************************************/
    
    StateGroup newAppControllerStates("AppControllerStates");
    newAppControllerStates.addState(State(kAPPCONTROLLER_INIT, "kAPPCONTROLLER_INIT"));
    newAppControllerStates.addState(State(kAPPCONTROLLER_PLAY, "kAPPCONTROLLER_PLAY"));
    
    appModel->addStateGroup(newAppControllerStates);
    
    StateGroup & appControllerStates = appModel->getStateGroup("AppControllerStates");
    
    /******************************************************
     *******                Config                  *******
     *****************************************************/
	Cunt c, d;

	cout << c.somecrap[20] << endl;

	Serializer.saveClass(ofToDataPath("test"), c, ARCHIVE_BINARY);
	Serializer.loadClass(ofToDataPath("test"), d, ARCHIVE_BINARY);

	cout << d.somecrap[20] << endl;



    appModel->load("config", ARCHIVE_BINARY);
    //appModel->removeAllProperties();
    
    //appModel->setupGui("AppModel", 1920 - 300, 0, 300, 0);
   // ofAddListener(appModel->getGui()->newGUIEvent, this, &AppController::guiEvent);
    
    //appModel->backup("config", ARCHIVE_BINARY);

	/*appModel->setProperty("LogToFile", false);
	appModel->setProperty("FullScreen", true);
	appModel->setProperty("SecondMonitor", true);
	appModel->setProperty("VerticalSync", false);
	appModel->setProperty("ShowCamera", false);
	appModel->setProperty("ShowBackground", false);
	appModel->setProperty("UseContour", true);
	appModel->setProperty("UseGPU", false);
	appModel->setProperty("UseSortBySize", false);
	appModel->setProperty("UseBackground", false);
	appModel->setProperty("UseRange", false);
	appModel->setProperty("UseInvertThresh", false);
	appModel->setProperty("UseApproxMode", true);
	appModel->setProperty("UseFindHoles", false);
	appModel->setProperty("UseVideo", true);
	appModel->setProperty("fLevel", 1.0f, 0.0f, 1.0f);
	appModel->setProperty("rLevel", 1.0f, 0.0f, 1.0f);
	appModel->setProperty("gLevel", 1.0f, 0.0f, 1.0f);
	appModel->setProperty("bLevel", 1.0f, 0.0f, 1.0f);
	appModel->setProperty("Threshold", 100, 0, 255);
	appModel->setProperty("Smooth", 0.5f, 0.0f, 1.0f);
	appModel->setProperty("Erode", 0, 0, 100);
	appModel->setProperty("Blur", 0, 0, 10);
	appModel->setProperty("MinContourSize", 50, 0, 4000);
	appModel->setProperty("MaxContourSize", 2000, 0, 4000);
	appModel->setProperty("ShowWarp", false);
	appModel->setProperty("ResetWarp", false);
	appModel->setProperty("UseWarp", true);
	appModel->setProperty("FrameRate", 50 , 1, 100);
	appModel->setProperty("Shutter", 20.0f, 1.0f, 100.0f);
	appModel->setProperty("Gain", 0.0f, 0.0f, 20.0f);

	appModel->setProperty("MediaPath", (string)"/Users/gameover/Desktop/LOTE/medianew");
    appModel->setProperty("VideoWidth", 1920.0f, 0.0f, 1920.0f);
	appModel->setProperty("VideoHeight", 1080.0f, 0.0f, 1080.0f);
    appModel->setProperty("OutputWidth", 1920.0f, 0.0f, 1920.0f);
    appModel->setProperty("OutputHeight", 1080.0f, 0.0f, 1080.0f);*/




	ofSetVerticalSync(appModel->getProperty<bool>("VerticalSync"));
	ofxLogSetLogToFile(appModel->getProperty<bool>("LogToFile"), ofToDataPath("log_" + ofGetTimestampString() + ".log"));

    /******************************************************
     *******                Views                   *******
     *****************************************************/
    
    // create appView windows
    //appView = new AppView();
    //appView->setup(appModel->getProperty<float>("OutputWidth"),
    //               appModel->getProperty<float>("OutputHeight"),
    //               ViewOption(VIEW_USE_BEZIERWARP), //VIEW_USE_FBO
    //               (string)"output");
    
    // make a debug window
    debugView = new DebugView();
    debugView->setup(ofGetWidth(),
                     ofGetHeight(),
                     ViewOption(),
                     (string)"debug");

    
    /******************************************************
     *******               Controllers              *******
     *****************************************************/
    cameraController = new CameraController;
	cameraController->setup();

    cameraController->setUseTexture(appModel->getProperty<bool>("ShowCamera"));
	cameraController->setVariables();

	if(appModel->hasProperty("WarpCoordinates")){ // assume we've saved a warp
		BezierWarp& warp = cameraController->getWarp();
		warp.allocate(	appModel->getProperty<float>("VideoWidth"), 
						appModel->getProperty<float>("VideoHeight"), 
						appModel->getProperty<int>("WarpNumXPoints"),
						appModel->getProperty<int>("WarpNumYPoints"),
						appModel->getProperty<float>("WarpGridResolution") );
		warp.setOffset(ofPoint(appModel->getProperty<float>("WarpOffsetX"), appModel->getProperty<float>("WarpOffsetY")));
		warp.setControlPoints(appModel->getProperty< vector<float> >("WarpCoordinates"));
	}

    /******************************************************
     *******              Mouse/Screen              *******
     *****************************************************/
    
//    CGPoint p;
//    p.x = 1980 * 2; p.y = 1200;
//    CGPostMouseEvent( p, 1, 1, 1 );
//    CGPostMouseEvent( p, 1, 1, 0 );
    
    //ofHideCursor();
    //ofSetFullscreen(true);

	//appModel->addListener<Toggle>("LogToFile", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("FullScreen", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("SecondMonitor", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("VerticalSync", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("ShowCamera", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("ShowBackground", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseContour", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseGPU", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseSortBySize", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseBackground", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseRange", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseInvertThresh", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseApproxMode", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseFindHoles", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseVideo", this, &AppController::parameterBoolEvent);
	appModel->addListener<FloatSlider>("fLevel", this, &AppController::parameterFloatEvent);
	appModel->addListener<FloatSlider>("rLevel", this, &AppController::parameterFloatEvent);
	appModel->addListener<FloatSlider>("gLevel", this, &AppController::parameterFloatEvent);
	appModel->addListener<FloatSlider>("bLevel", this, &AppController::parameterFloatEvent);
	appModel->addListener<IntSlider>("Threshold", this, &AppController::parameterIntEvent);
	appModel->addListener<FloatSlider>("Smooth", this, &AppController::parameterFloatEvent);
	appModel->addListener<IntSlider>("Erode", this, &AppController::parameterIntEvent);
	appModel->addListener<IntSlider>("Blur", this, &AppController::parameterIntEvent);
	appModel->addListener<IntSlider>("MinContourSize", this, &AppController::parameterIntEvent);
	appModel->addListener<IntSlider>("MaxContourSize", this, &AppController::parameterIntEvent);
	appModel->addListener<Toggle>("ShowWarp", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("ResetWarp", this, &AppController::parameterBoolEvent);
	appModel->addListener<Toggle>("UseWarp", this, &AppController::parameterBoolEvent);
	appModel->addListener<IntSlider>("FrameRate", this, &AppController::parameterIntEvent);
	appModel->addListener<FloatSlider>("Shutter", this, &AppController::parameterFloatEvent);
	appModel->addListener<FloatSlider>("Gain", this, &AppController::parameterFloatEvent);

    cameraController->setVariables();
	ofBackground(0, 0, 0);
    appControllerStates.setState(kAPPCONTROLLER_INIT);

	//assert(false);
}

//--------------------------------------------------------------
void AppController::parameterIntEvent(ParameterEvent<int> &e){
	cout << e.name << ": " << (*e.value) << endl;
	if(e.name == "FrameRate") cameraController->setCameraFrameRate((*e.value));
	cameraController->setVariables();
}

//--------------------------------------------------------------
void AppController::parameterFloatEvent(ParameterEvent<float> &e){
	cout << e.name << ": " << (*e.value) << endl;
	if(e.name == "Gain") cameraController->setCameraGain((*e.value));
	if(e.name == "Shutter") cameraController->setCameraShutter((*e.value));
	cameraController->setVariables();
}
//--------------------------------------------------------------
void AppController::parameterBoolEvent(ParameterEvent<bool> &e){
	cout << e.name << ": " << (*e.value) << endl;
	if(e.name == "ShowCamera") cameraController->setUseTexture((*e.value));
	if(e.name == "VerticalSync") ofSetVerticalSync((*e.value));
	if(e.name == "FullScreen") setFullScreen((*e.value));
	if(e.name == "LogToFile") ofxLogSetLogToFile((*e.value), ofToDataPath("log_" + ofGetTimestampString() + ".log"));
	if(e.name == "ResetWarp" && (*e.value) == true){
		appModel->setProperty("ResetWarp", false);
		cameraController->getWarp().resetWarpGrid();
	}
	cameraController->setVariables();
}

//--------------------------------------------------------------
void AppController::update(){
    
    StateGroup & appControllerStates = appModel->getStateGroup("AppControllerStates");
    
    switch (appControllerStates.getState()) {
            
        case kAPPCONTROLLER_INIT:
        {
			if(ofGetFrameNum() > 1){ // ensure we've started the gl context/windowing
				setFullScreen(appModel->getProperty<bool>("FullScreen"));
				appControllerStates.setState(kAPPCONTROLLER_PLAY);
			}
        }
            break;
        case kAPPCONTROLLER_PLAY:
        {
			appModel->getGui()->update();
            cameraController->update();
        }
            break;
    }
    
}

//--------------------------------------------------------------
void AppController::draw(){
    
    StateGroup & debugViewStates = appModel->getStateGroup("DebugViewStates");
    StateGroup & appControllerStates = appModel->getStateGroup("AppControllerStates");

    //appView->update();
    
    if(debugViewStates.getState(kDEBUGVIEW_SHOWINFO)) debugView->update();
    
    switch (appControllerStates.getState()) {
        case kAPPCONTROLLER_INIT:
        {
            // nothing to do
        }
            break;
        case kAPPCONTROLLER_PLAY:
        {

            ofEnableBlendMode(OF_BLENDMODE_SCREEN);

			cameraController->draw();

        }
            break;
    }
    
    if(debugViewStates.getState(kDEBUGVIEW_SHOWINFO)) debugView->draw();
    
    ofDisableBlendMode();
    
}

//--------------------------------------------------------------
void AppController::exit(){
    appModel->save("config", ARCHIVE_BINARY);
    delete debugView;
	//delete cameraController;
}

//--------------------------------------------------------------
void AppController::keyPressed(ofKeyEventArgs & e){
    
    StateGroup & debugViewStates = appModel->getStateGroup("DebugViewStates");
    StateGroup & appViewStates = appModel->getStateGroup("AppViewStates");
    
    //if(appModel->getGui()->hasKeyboardFocus()) return;
    
    switch(e.key) {
        case 'g':
            debugViewStates.toggleState(kDEBUGVIEW_SHOWGUI);
			appModel->getGui()->setVisible(debugViewStates.getState(kDEBUGVIEW_SHOWGUI));
            break;
        case 'd':
            debugViewStates.toggleState(kDEBUGVIEW_SHOWINFO);
            break;
        case 'p':
            debugViewStates.toggleState(kDEBUGVIEW_SHOWPROPS);
            break;
        case 's':
            debugViewStates.toggleState(kDEBUGVIEW_SHOWSTATES);
            break;
        case 'b':
            cameraController->setBackground();
            break;
		case ' ':
			cameraController->setVariables();
            //cameraController->setLatencyTest();
            break;
		case 'r':
			{
				if(cameraController->isRecording()){
					cameraController->setRecord(false);
				}else{
					string fileName = "cam_" + ofGetTimestampString() + ".avi";
					cameraController->setRecordPath(fileName);
					cameraController->setRecordType(UNCOMPRESSED);
					cameraController->setRecord(true);
				}
				appModel->setProperty("Recording", cameraController->isRecording());
			}
			
			break;
	}
    
}

//--------------------------------------------------------------
void AppController::keyReleased(ofKeyEventArgs & e){

}

//--------------------------------------------------------------
void AppController::mouseMoved(ofMouseEventArgs & e){

}

//--------------------------------------------------------------
void AppController::mouseDragged(ofMouseEventArgs & e){
    //if(keyModifiers.getAppleCommandModifier()){
        //appModel->getGui()->setPosition(e.x, e.y);
    //}
}

//--------------------------------------------------------------
void AppController::mousePressed(ofMouseEventArgs & e){
	if(e.button == 0) return;
	cameraController->setColorPick(e.x, e.y);
}

//--------------------------------------------------------------
void AppController::mouseReleased(ofMouseEventArgs & e){
    if(appModel->getProperty<bool>("ShowWarp")){
		BezierWarp & warp = cameraController->getWarp();
		appModel->setProperty("WarpNumXPoints", warp.getNumXPoints());
		appModel->setProperty("WarpNumYPoints", warp.getNumYPoints());
		appModel->setProperty("WarpGridResolution", warp.getWarpGridResolution());
		appModel->setProperty("WarpOffsetX", warp.getOffsetReference().x);
		appModel->setProperty("WarpOffsetY", warp.getOffsetReference().y);
		appModel->setProperty("WarpCoordinates", warp.getControlPointsReference());
	}
}

//--------------------------------------------------------------
std::wstring string2wString(const std::string& s){
	
	int slength = (int)s.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete [] buf;
	return r;

}

//--------------------------------------------------------------
void AppController::setFullScreen(bool b){

	//ofSetFullscreen(b);

	string windowName = "ADT";
	ofSetWindowTitle(windowName);

	int x = 0;
	int y = 0;
	int width = 1920.0f * 2.0f;
	int height = 1080.0f;
	std::wstring w = string2wString(windowName);
	LPCWSTR lpcWindowName = w.c_str();
	HWND vWnd  = FindWindow(NULL, lpcWindowName);
    long windowStyle = GetWindowLong(vWnd, GWL_STYLE);

	if(b){ // go fullscreen
        windowStyle &= ~WS_OVERLAPPEDWINDOW;
        windowStyle |= WS_POPUP;
	}else{ // go window
        windowStyle |= WS_TILEDWINDOW;
	}

	SetWindowLong(vWnd, GWL_STYLE, windowStyle);
    SetWindowPos(vWnd, HWND_TOP, x, y, width, height, SWP_FRAMECHANGED);

}

