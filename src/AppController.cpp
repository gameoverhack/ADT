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
    ofxLogNotice() << "Creating AppController" << endl << "more??";
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
    appModel->removeAllProperties();
    
    appModel->setupGui("AppModel", 600, 0, 300, 0);
    ofAddListener(appModel->getGui()->newGUIEvent, this, &AppController::guiEvent);
    
    //appModel->backup("config", ARCHIVE_BINARY);

    appModel->setProperty("LogToFile", false, true);
    appModel->setProperty("FullScreen", false, true);
    appModel->setProperty("VerticalSync", false, true);
    appModel->setProperty("Ortho", true, true);
    appModel->setProperty("MediaPath", (string)"/Users/gameover/Desktop/LOTE/medianew", true);
    appModel->setProperty("VideoWidth", 1920.0f, true, 0.0f, 1920.0f);
    appModel->setProperty("VideoHeight", 1080.0f, true, 0.0f, 1080.0f);
    appModel->setProperty("OutputWidth", 1920.0f, true, 0.0f, 1920.0f);
    appModel->setProperty("OutputHeight", 1080.0f, true, 0.0f, 1080.0f);
    
    ofSetWindowShape(appModel->getProperty<float>("OutputWidth"), appModel->getProperty<float>("OutputHeight"));
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
    
    
    /******************************************************
     *******              Mouse/Screen              *******
     *****************************************************/
    
//    CGPoint p;
//    p.x = 1980 * 2; p.y = 1200;
//    CGPostMouseEvent( p, 1, 1, 1 );
//    CGPostMouseEvent( p, 1, 1, 0 );
    
    //ofHideCursor();
    //ofSetFullscreen(true);
    
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
            
        }
            break;
        case OFX_UI_WIDGET_SLIDER_H:
            ofxLogVerbose() << "UI event " << name << " (f) slider: " << ((ofxUISlider *)e.widget)->getValue() << endl;
        {
            if(name == "OutputWidth" || name == "OutputHeight") ofSetWindowShape(appModel->getProperty<float>("OutputWidth"), appModel->getProperty<float>("OutputHeight"));
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
            if(name == "VerticalSync") ofSetVerticalSync(appModel->getProperty<bool>("VerticalSync"));
            if(name == "FullScreen") ofSetFullscreen(appModel->getProperty<bool>("FullScreen"));
            if(name == "LogToFile") ofxLogSetLogToFile(appModel->getProperty<bool>("LogToFile"), ofToDataPath("log_" + ofGetTimestampString() + ".log"));
        }
            
            break;
            
        default:
            ofxLogWarning() << "No such ofxUI kind" << endl;
            break;
    }
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
            
        }
            break;
    }
    
}

//--------------------------------------------------------------
void AppController::draw(){
    
    StateGroup & debugViewStates = appModel->getStateGroup("DebugViewStates");
    StateGroup & appControllerStates = appModel->getStateGroup("AppControllerStates");

    appView->update();
    
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

            appView->draw();
            
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
        case 'c':
            appView->resetCamera();
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
        appModel->getGui()->setPosition(e.x, e.y);
    //}
}

//--------------------------------------------------------------
void AppController::mousePressed(ofMouseEventArgs & e){

}

//--------------------------------------------------------------
void AppController::mouseReleased(ofMouseEventArgs & e){
    
}