//
//  AppController.cpp
//  LAFTest
//
//  Created by gameover on 9/01/14.
//
//

#include "AppController.h"

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
    
    appModel->load("config", ARCHIVE_BINARY);
    //appModel->removeAllProperties();
    
    appModel->setupGui("AppModel", 1920 - 300, 0, 300, 0);
    ofAddListener(appModel->getGui()->newGUIEvent, this, &AppController::guiEvent);
    
    //appModel->backup("config", ARCHIVE_BINARY);

    appModel->setProperty("LogToFile", false, true);
    appModel->setProperty("FullScreen", false, true);
    appModel->setProperty("VerticalSync", false, true);
	//appModel->setProperty("ShowCamera", false, true);
	//appModel->setProperty("UseContour", true, true);
	//appModel->setProperty("UseGPU", false, true);
	//appModel->setProperty("UseSortBySize", false, true);
	//appModel->setProperty("UseBackground", false, true);
	//appModel->setProperty("UseInvertThresh", false, true);
	//appModel->setProperty("UseApproxMode", true, true);
	//appModel->setProperty("UseFindHoles", false, true);
	//appModel->setProperty("Points", 500 , true, 100, 2000);
	//appModel->setProperty("Threshold", 100 , true, 0, 255);
	//appModel->setProperty("Smooth", 0.5f , true, 0.0f, 1.0f);
	//appModel->setProperty("Erode", 0, true, 0, 100);
	//appModel->setProperty("Blur", 0 , true, 0, 10);
	//appModel->setProperty("MinContourSize", 200 , true, 0, 4000);
	//appModel->setProperty("MaxContourSize", 2000, true, 0, 4000);
	appModel->setProperty("FrameRate", 50 , true, 1, 100);
	appModel->setProperty("Shutter", 5.0f, true, 1.0f, 100.0f);
	appModel->setProperty("Gain", 0.0, true, 0.0f, 20.0f);
    appModel->setProperty("MediaPath", (string)"/Users/gameover/Desktop/LOTE/medianew", true);
    appModel->setProperty("VideoWidth", 1920.0f, true, 0.0f, 1920.0f);
    appModel->setProperty("VideoHeight", 1080.0f, true, 0.0f, 1080.0f);
    appModel->setProperty("OutputWidth", 1920.0f, false, 0.0f, 1920.0f);
    appModel->setProperty("OutputHeight", 1080.0f, false, 0.0f, 1080.0f);
    



    //ofSetWindowShape(appModel->getProperty<float>("OutputWidth"), appModel->getProperty<float>("OutputHeight"));
    ofSetVerticalSync(appModel->getProperty<bool>("VerticalSync"));
    ofSetFullscreen(appModel->getProperty<bool>("FullScreen"));
    ofxLogSetLogToFile(appModel->getProperty<bool>("LogToFile"), ofToDataPath("log_" + ofGetTimestampString() + ".log"));
    
    /******************************************************
     *******                Views                   *******
     *****************************************************/
    
    // create appView windows
    appView = new AppView();
    appView->setup(appModel->getProperty<float>("OutputWidth"),
                   appModel->getProperty<float>("OutputHeight"),
                   ViewOption(), //VIEW_USE_FBO
                   (string)"output");
    
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
    
    /******************************************************
     *******              Mouse/Screen              *******
     *****************************************************/
    
//    CGPoint p;
//    p.x = 1980 * 2; p.y = 1200;
//    CGPostMouseEvent( p, 1, 1, 1 );
//    CGPostMouseEvent( p, 1, 1, 0 );
    
    //ofHideCursor();
    //ofSetFullscreen(true);
    
	ofBackground(0, 0, 0);
    appControllerStates.setState(kAPPCONTROLLER_INIT);
    
}

//--------------------------------------------------------------
void AppController::guiEvent(ofxUIEventArgs &e){
    
    string name = e.getName();
	int kind = e.getKind();
    
    switch (kind) {
        case OFX_UI_WIDGET_INTSLIDER_H:
            ofxLogVerbose() << "UI event " << name << " (i) slider: " << ((ofxUIIntSlider *)e.widget)->getValue() << endl;
        {
            if(name == "FrameRate") cameraController->setCameraFrameRate(appModel->getProperty<int>("FrameRate"));
        }
            break;
        case OFX_UI_WIDGET_SLIDER_H:
            ofxLogVerbose() << "UI event " << name << " (f) slider: " << ((ofxUISlider *)e.widget)->getValue() << endl;
        {
			 if(name == "Gain") cameraController->setCameraGain(appModel->getProperty<float>("Gain"));
			 if(name == "Shutter") cameraController->setCameraShutter(appModel->getProperty<float>("Shutter"));
            //if(name == "OutputWidth" || name == "OutputHeight") ofSetWindowShape(appModel->getProperty<float>("OutputWidth"), appModel->getProperty<float>("OutputHeight"));
        }
            break;
        case OFX_UI_WIDGET_TEXTINPUT:
            ofxLogVerbose() << "UI event " << name << " (s) string: " << ((ofxUITextInput *)e.widget)->getTextString() << " (" << ((ofxUITextInput *)e.widget)->getInputTriggerType() << ")" << endl;
        {
            
        }
            break;
        case OFX_UI_WIDGET_TOGGLE:
            ofxLogVerbose() << "UI event " << name << " (b) toggle: " << ((ofxUIToggle *)e.widget)->getValue() << endl;
        {
			if(name == "ShowCamera") cameraController->setUseTexture(appModel->getProperty<bool>("ShowCamera"));
            if(name == "VerticalSync") ofSetVerticalSync(appModel->getProperty<bool>("VerticalSync"));
            if(name == "FullScreen") ofSetFullscreen(appModel->getProperty<bool>("FullScreen"));
            if(name == "LogToFile") ofxLogSetLogToFile(appModel->getProperty<bool>("LogToFile"), ofToDataPath("log_" + ofGetTimestampString() + ".log"));
        }
            
            break;
            
        default:
            ofxLogWarning() << "No such ofxUI kind" << endl;
            break;
    }
	cameraController->setVariables();
}

//--------------------------------------------------------------
void AppController::update(){
    
    StateGroup & appControllerStates = appModel->getStateGroup("AppControllerStates");
    
    switch (appControllerStates.getState()) {
            
        case kAPPCONTROLLER_INIT:
        {
            appControllerStates.setState(kAPPCONTROLLER_PLAY);
        }
            break;
        case kAPPCONTROLLER_PLAY:
        {
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

            //appView->draw();
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
    delete appView;
    delete debugView;
}

//--------------------------------------------------------------
void AppController::keyPressed(ofKeyEventArgs & e){
    
    StateGroup & debugViewStates = appModel->getStateGroup("DebugViewStates");
    StateGroup & appViewStates = appModel->getStateGroup("AppViewStates");
    
    if(appModel->getGui()->hasKeyboardFocus()) return;
    
    switch(e.key) {
        case 'g':
            debugViewStates.toggleState(kDEBUGVIEW_SHOWGUI);
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
            cameraController->setLatencyTest();
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

}

//--------------------------------------------------------------
void AppController::mouseReleased(ofMouseEventArgs & e){
    
}