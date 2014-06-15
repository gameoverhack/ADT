//
//  AppController.cpp
//  LAFTest
//
//  Created by gameover on 9/01/14.
//
//

#include "AppController.h"
//#include "SerializationExports.h"

#define NUM_TOGGLES 64
//vector<Parameter<bool>*> tests;
//vector<bool> testS;
//
vector<float> sliderValues;

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
    appModel->removeAllProperties();
    
//    appModel->setupGui("AppModel", 600, 0, 300, 0);
//    ofAddListener(appModel->getGui()->newGUIEvent, this, &AppController::guiEvent);
    
    //appModel->backup("config", ARCHIVE_BINARY);

    appModel->setProperty("LogToFile", false);
    appModel->setProperty("FullScreen", false);
    appModel->setProperty("VerticalSync", false);
    appModel->setProperty("Ortho", true);
    appModel->setProperty("MediaPath", (string)"/Users/gameover/Desktop/LOTE/medianew");
    //appModel->setProperty("VideoWidth", 1920.0f, true, 0.0f, 1920.0f);
    //appModel->setProperty("VideoHeight", 1080.0f, true, 0.0f, 1080.0f);
    
    for(int i = 1; i < 17; i++){
        appModel->setProperty("DMXLevel_" + ofToString(i), 0, 0, 255);
    }
    
    appModel->setProperty("OutputWidth", 1920.0f, 0.0f, 1920.0f);
    appModel->setProperty("OutputHeight", 1080.0f, 0.0f, 1080.0f);
    
    ofSetWindowShape(appModel->getPropertyReference<float>("OutputWidth"), appModel->getPropertyReference<float>("OutputHeight"));
    ofSetVerticalSync(appModel->getPropertyReference<bool>("VerticalSync"));
    ofSetFullscreen(appModel->getPropertyReference<bool>("FullScreen"));
    ofxLogSetLogToFile(appModel->getPropertyReference<bool>("LogToFile"), ofToDataPath("log_" + ofGetTimestampString() + ".log"));
    
    /******************************************************
     *******                Views                   *******
     *****************************************************/
    
    // create appView windows

    
    // make a debug window
    debugView = new DebugView();
    debugView->setup(ofGetWidth(),
                     ofGetHeight(),
                     ViewOption(),
                     (string)"debug");

     
    /******************************************************
     *******               Controllers              *******
     *****************************************************/
    
   // deviceController = new DeviceController;
    
    /******************************************************
     *******              Mouse/Screen              *******
     *****************************************************/
    
//    CGPoint p;
//    p.x = 1980 * 2; p.y = 1200;
//    CGPostMouseEvent( p, 1, 1, 1 );
//    CGPostMouseEvent( p, 1, 1, 0 );
    
    //ofHideCursor();
    //ofSetFullscreen(true);
    
    appModel->setProperty("SomeInt", 1);
    appModel->setProperty("SomeFloat", 2.0f);
    appModel->setProperty("SomeString", (string)"string&*^% {++");
    appModel->setProperty("SomeBool", true, true);
    appModel->setProperty("SomeChar", 'r');
    appModel->setProperty("SomePoint", ofPoint(1,2,3));
    appModel->setProperty("SomeRect", ofRectangle(0,0,320,240));
    appModel->setProperty("SomeColor", ofColor(127,0,127,0));
    
    for(int i = 0; i < 100; i++){
        appModel->setProperty("SomeIntMinMax" + ofToString(i), 50, 0, 100, true);
        appModel->setProperty("SomeFloatMinMax" + ofToString(i), &lafloat[i], 0.0f, 1.0f, true);
        lafloat[i] = ofRandom(1.0);
    }
    
    
    
    
    
    appModel->setProperty("SomeInt", 100);
    appModel->setProperty("SomeFloat", 20.0f);
    appModel->setProperty("SomeString", (string)"anotherstring&*^% {++");

    cout << appModel->getPropertyReference<int>("SomeInt") << endl;
    cout << appModel->getPropertyReference<int>("SomeInt") << endl;
    
    cout << appModel->getPropertyReference<ofPoint>("SomePoint") << endl;
    cout << appModel->getPropertyReference<ofPoint>("SomePoint") << endl;

    
    
    
    vector<int> intVec;
    for(int i = 0; i < 4; i++){
        intVec.push_back(i);
    }

    for(int i = 0; i < 10000; i++){
        appModel->setProperty("SomeIntVec_" + ofToString(i), intVec);
        appModel->setProperty("SomeInt_" + ofToString(i), (int)ofRandom(1000));
        appModel->setProperty("SomeIntVec_" + ofToString(i), intVec);
        appModel->setProperty("SomeInt_" + ofToString(i), (int)ofRandom(1000));
    }
    
    appModel->setProperty("SomeIntVec", (vector<int>)intVec);
    appModel->setProperty("SomeInt", (int)ofRandom(1000));
    appModel->setProperty("SomeIntVec", intVec);
    appModel->setProperty("SomeInt", (int)ofRandom(1000));
    
    vector<int> aintVec;
    int aint;
    
    
    appModel->setProperty("SomeIntVec_5000", &laintVec);
    appModel->setProperty("SomeInt_5000", &laint);

    
    for(int k = 0; k < 4; k++){
        cout << "    test: " << k << endl;
        
        int startTime = ofGetElapsedTimeMillis();
        for(int i = 0; i < 1000000; i++){
            aintVec = intVec;
            aint = (int)ofRandom(1000);
            //appModel->removeProperty2("SomeInt");
            //appModel->removeProperty2("SomeIntVec");
        }
        cout << "Total time: " << ofGetElapsedTimeMillis() - startTime << endl;
        
        
        startTime = ofGetElapsedTimeMillis();
        for(int i = 0; i < 1000000; i++){
            laintVec = intVec;
            laint = (int)ofRandom(1000);
            //appModel->removeProperty2("SomeInt");
            //appModel->removeProperty2("SomeIntVec");
        }
        cout << "Total time: " << ofGetElapsedTimeMillis() - startTime << endl;
        
//        cout << appModel->getProperty< vector<int> >("SomeIntVec2") << endl;
//        cout << appModel->getProperty< int >("SomeInt2") << endl;
        

        Parameter< vector<int> >* qaintVec = appModel->getProperty< vector<int> >("SomeIntVec");
        Parameter< int >* qaint = appModel->getProperty< int >("SomeInt");
        
        startTime = ofGetElapsedTimeMillis();
        for(int i = 0; i < 1000000; i++){
            qaintVec->set(intVec);
            qaint->set((int)ofRandom(1000));
            //appModel->removeProperty2("SomeInt");
            //appModel->removeProperty2("SomeIntVec");
        }
        cout << "Total time: " << ofGetElapsedTimeMillis() - startTime << endl;
        
//        cout << appModel->getProperty< int >("SomeInt") << endl;

        
        startTime = ofGetElapsedTimeMillis();
        for(int i = 0; i < 1000000; i++){
            
            appModel->setProperty("SomeIntVec_9000", intVec);
            appModel->setProperty("SomeInt_9000", (int)ofRandom(1000));
            //appModel->removeProperty2("SomeInt");
            //appModel->removeProperty2("SomeIntVec");
        }
        cout << "Total time: " << ofGetElapsedTimeMillis() - startTime << endl;
        
        startTime = ofGetElapsedTimeMillis();
        for(int i = 0; i < 1000000; i++){
            appModel->setProperty("SomeIntVec_9000", intVec);
            appModel->setProperty("SomeInt_9000", (int)ofRandom(1000));
            //appModel->removeProperty< int >("SomeInt");
            //appModel->removeProperty< vector<int> >("SomeIntVec");
        }
        cout << "Total time: " << ofGetElapsedTimeMillis() - startTime << endl;
    }

    
    
    
    
//    appModel->setProperty("SomeIntVec", intVec);
//    
//    cout << appModel->hasProperty2("SomeIntVec") << endl;
//    cout << appModel->hasProperty2("Error") << endl;
//    
//    bool b;
//    cout << typeid(bool).name() << endl;
//    
//    cout << appModel->getProperty<bool>("SomeBool") << endl;
//    appModel->toggleProperty2("SomeBool");
//    cout << appModel->getProperty<bool>("SomeBool") << endl;
//    
//    cout << appModel->getProperty< vector<int> >("SomeIntVec") << endl;
//    
////    appModel->toggleProperty2("SomeIntVec");
//    
//    appModel->removeProperty2("SomeIntVec");
    
    
    
    
//
//    
////    cout << appModel->getProperty<bool>("SomeIntVec") << endl;
////    
////    cout << appModel->getProperty<int>("Error") << endl;
//    
//    
//    map<string, BaseParameter*> parameters;
//    Parameter<int>* p1 = new Parameter<int>("SomeInt", 1);
//    Parameter<float>* p2 = new Parameter<float>("SomeFloat", 2.0);
//    Parameter<string>* p3 = new Parameter<string>("SomeString", "string&*^% {++");
//    Parameter<bool>* p4 = new Parameter<bool>("SomeBool", true);
//    Parameter<char>* p5 = new Parameter<char>("SomeChar", 'r');
//    Parameter<ofPoint>* p6 = new Parameter<ofPoint>("SomePoint", ofPoint(1,2,3));
//    Parameter<ofRectangle>* p7 = new Parameter<ofRectangle>("SomeRect", ofRectangle(0,0,320,240));
//    Parameter<ofColor>* p8 = new Parameter<ofColor>("SomeColor", ofColor(127,0,127,0));
//
//    parameters["p1"] = p1;
//    parameters["p2"] = p2;
//    parameters["p3"] = p3;
//    parameters["p4"] = p4;
//    parameters["p5"] = p5;
//    parameters["p6"] = p6;
//    parameters["p7"] = p7;
//    parameters["p8"] = p8;
//
//    Serializer.saveClass(ofToDataPath("test"), appModel->parameters, ARCHIVE_BINARY);
//
//    map<string, BaseParameter*> parametersl;
//
//    Serializer.loadClass(ofToDataPath("test"), parametersl, ARCHIVE_BINARY);
//
//    for(map<string, BaseParameter*>::iterator it = parametersl.begin(); it != parametersl.end(); ++it){
//        BaseParameter* bp = (BaseParameter*)it->second;
//        cout << it->first << " " << endl << bp->toString() << endl;
//    }
    

//    guiO.setup();
//    
//    
//    guiC = new ofxUISuperCanvas("gui", 300, 0, 1920, 1080);
//    //ofxUIToggleMatrix * matrixC = new ofxUIToggleMatrix(200, 200, 10, 10, "matrix");
//    guiC->addToggleMatrix("matrix", 16, 16);
//    
//    //guiC->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN, OFX_UI_ALIGN_FREE);
//    
//    sliderValues.resize(200);
//    for(int i = 0; i < 50; i++){
//        sliderValues[i] = ofRandom(100);
//        guiC->addSlider("slider_" + ofToString(i), 0, 100, &sliderValues[i], 300, 10, 0, i*20);
//        guiO.add(ss[i].setup("slider_" + ofToString(50), sliderValues[i], 0, 100));
//    }
//    
//    for(int i = 0; i < 50; i++){
//        sliderValues[i + 50] = ofRandom(100);
//        ofxUISlider * s = new ofxUISlider("slider_" + ofToString(i + 50), 0, 100, &sliderValues[i+50], 300, 10, 300, i*20);
//        guiC->addWidget(s);
//        guiO.add(ss[i+50].setup("slider_" + ofToString(i + 50), sliderValues[i+50], 0, 100));
//    }
//
//    for(int i = 0; i < 50; i++){
//        sliderValues[i + 100] = ofRandom(100);
//        ofxUISlider * s = new ofxUISlider("slider_" + ofToString(i + 100), 0, 100, &sliderValues[i+100], 300, 10, 600, i*20);
//        guiC->addWidget(s);
//        guiO.add(ss[i+100].setup("slider_" + ofToString(i + 100), sliderValues[i+100], 0, 100));
//    }
//    for(int i = 0; i < 50; i++){
//        sliderValues[i + 150] = ofRandom(100);
//        ofxUISlider * s = new ofxUISlider("slider_" + ofToString(i + 150), 0, 100, &sliderValues[i+150], 300, 10, 900, i*20);
//        guiC->addWidget(s);
//        guiO.add(ss[i+150].setup("slider_" + ofToString(i + 150), sliderValues[i+150], 0, 100));
//    }
//    
//    gui = new Gui;
//    gui->setResizeMode(BaseWidget::RESIZEMODE_ABS);
//    gui->setup(100, 0, 1920, 1080);
//    gui->setLabel("GUI");
//    //gui->setPosition(100, 100);
//    
//    Matrix* matrix = new Matrix;
//    matrix->setPosition(0, 0);
//    matrix->setDimensions(16, 16);
//    matrix->setLabel("matrix");
//    gui->add(matrix);
//    
//
//    
//    Canvas* imageCanvas = new Canvas;
//    imageCanvas->setResizeMode(BaseWidget::RESIZEMODE_GROW);
//    //imageCanvas->setup(0, 0, 550, 550);
//    imageCanvas->setLabel("image");
//    
//    Image* image = new Image;
//    image->setup(0, 0, 550, 550);
//    image->setImage(&vid.getTextureReference());
//    
//    imageCanvas->add(image);
//    
//    gui->add(imageCanvas);
//    
//    Canvas* toggleCanvas = new Canvas;
//    toggleCanvas->setResizeMode(BaseWidget::RESIZEMODE_ABS);
//    toggleCanvas->setLabel("toggles");
//    toggleCanvas->setup(0, matrix->getHeight(), 300, 400);
//
//    for(int i = 0; i < 100; i++){
//        Toggle* toggle = new Toggle;
//        toggle->setLabel("toggle_" + ofToString(i));
//        toggle->setup(0, 20 * i, 300, 24);
//        toggle->set((bool)floor(ofRandom(2)));
//        toggleCanvas->add(toggle);
//    }
//    gui->add(toggleCanvas);
//    
//    List* list = new List;
//    list->setResizeMode(BaseWidget::RESIZEMODE_ABS);
//    list->setLabel("list");
//    list->setup(0, matrix->getHeight() + 400, 300, 400);
//    vector<string> tItems;
//    
//    for(int i = 0; i < 100; i++){
//        tItems.push_back("item_" + ofToString(i));
//    }
//    
//    list->setList(tItems);
//    
//    gui->add(list);
//    
//
//    
//    Canvas* sliderCanvas = new Canvas;
//    sliderCanvas->setResizeMode(BaseWidget::RESIZEMODE_GROW);
//    sliderCanvas->setLabel("sliders");
//    sliderCanvas->setPosition(matrix->getWidth(), 0);
//    
//    sliderValues.resize(100);
//    
//    for(int i = 0; i < 50; i++){
//        Slider* slider = new Slider;
//        slider->setLabel("slider_" + ofToString(i));
//        
//        
//        slider->setup(0, 20 * i, 300, 24);
//        slider->setRange(0, 100);
//        slider->set(&sliderValues[i + 0], true);
//        
//        sliderCanvas->add(slider);
//    }
//    
//    for(int i = 0; i < 50; i++){
//        Slider* slider = new Slider;
//        slider->setLabel("slider_" + ofToString(i + 50));
//        slider->setup(304, 20 * i, 300, 24);
//        slider->setRange(0, 100);
//
//        slider->set(&sliderValues[i + 50], true);
//        
//        sliderCanvas->add(slider);
//    }
//    
//    for(int i = 0; i < 50; i++){
//        Slider* slider = new Slider;
//        slider->setLabel("slider_" + ofToString(i + 100));
//        slider->setup(608, 20 * i, 300, 24);
//        slider->setRange(0, 100);
//        slider->set(&sliderValues[i + 100], true);
//        
//        sliderCanvas->add(slider);
//    }
//    
//    for(int i = 0; i < 50; i++){
//        Slider* slider = new Slider;
//        slider->setLabel("slider_" + ofToString(i + 150));
//        slider->setup(912, 20 * i, 300, 24);
//        slider->setRange(0, 100);
//        
//        slider->set(&sliderValues[i + 150], true);
//        
//        sliderCanvas->add(slider);
//    }
//    
//    gui->add(sliderCanvas);
    appModel->save("config", ARCHIVE_BINARY);
    
    //cout << appModel->getAllPropsAsString();
    
    appControllerStates.setState(kAPPCONTROLLER_INIT);
    
}

////--------------------------------------------------------------
//void AppController::guiEvent(ofxUIEventArgs &e){
//    
//    string name = e.getName();
//	int kind = e.getKind();
//    
//    switch (kind) {
//        case OFX_UI_WIDGET_INTSLIDER_H:
//            ofxLogVerbose() << "UI event " << name << " (i) slider: " << ((ofxUIIntSlider *)e.widget)->getValue() << endl;
//        {
//            
//        }
//            break;
//        case OFX_UI_WIDGET_SLIDER_H:
//            ofxLogVerbose() << "UI event " << name << " (f) slider: " << ((ofxUISlider *)e.widget)->getValue() << endl;
//        {
//            if(name == "OutputWidth" || name == "OutputHeight") ofSetWindowShape(appModel->getProperty<float>("OutputWidth"), appModel->getProperty<float>("OutputHeight"));
//        }
//            break;
//        case OFX_UI_WIDGET_TEXTINPUT:
//            ofxLogVerbose() << "UI event " << name << " (s) string: " << ((ofxUITextInput *)e.widget)->getTextString() << " (" << ((ofxUITextInput *)e.widget)->getInputTriggerType() << ")" << endl;
//        {
//            
//        }
//            break;
//        case OFX_UI_WIDGET_TOGGLE:
//            ofxLogVerbose() << "UI event " << name << " (b) toggle: " << ((ofxUIToggle *)e.widget)->getValue() << endl;
//        {
//            if(name == "VerticalSync") ofSetVerticalSync(appModel->getProperty<bool>("VerticalSync"));
//            if(name == "FullScreen") ofSetFullscreen(appModel->getProperty<bool>("FullScreen"));
//            if(name == "LogToFile") ofxLogSetLogToFile(appModel->getProperty<bool>("LogToFile"), ofToDataPath("log_" + ofGetTimestampString() + ".log"));
//        }
//            
//            break;
//            
//        default:
//            ofxLogWarning() << "No such ofxUI kind" << endl;
//            break;
//    }
//}

//--------------------------------------------------------------
void AppController::update(){
    
    StateGroup & appControllerStates = appModel->getStateGroup("AppControllerStates");
    
    switch (appControllerStates.getState()) {
            
        case kAPPCONTROLLER_INIT:
        {
            //deviceController->setup();
            //vid.loadMovie("/Users/gameover/Desktop/LOTE/media/BLADIMIRSL/CRCH_FRNT_CLIM_DOWN_STND_BACK_00_BLADIMIRSL.mov");
            //vid.play();
            appControllerStates.setState(kAPPCONTROLLER_PLAY);
        }
            break;
        case kAPPCONTROLLER_PLAY:
        {
            //vid.update();
            appModel->getGui()->update();
            //guiC->update();
            //deviceController->update();
        }
            break;
    }
    
}
static bool bizzy = true;
//--------------------------------------------------------------
void AppController::draw(){
    
    ofBackground(0, 0, 0);
    
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
            
            if(bizzy){
                for(int i = 0; i < 100; i++){
                    appModel->setProperty("SomeIntMinMax" + ofToString(i), (int)ofRandom(100));
                    lafloat[i] = ofRandom(1.0);
                }
                
                
            }
//
//                Matrix* matrix = (Matrix*)gui->get("matrix");
//                for(int i = 0; i < matrix->rows(); i++){
//                    for(int j = 0; j < matrix->cols(); j++){
//                        CheckBox* checkBox = matrix->at(i, j);
//                        checkBox->set((bool)floor(ofRandom(2)));
//                    }
//                }
//                
//                for(int i = 0; i < 200; i++){
//                    //Slider* slider = (Slider*)gui->get("slider_" + ofToString(i));
//                    //slider->set(ofRandom(100));
//                    sliderValues[i] = ofRandom(100);
//                }
//                
//
//            }
            
            

            //appView->draw();
            appModel->getGui()->draw();
//            if(!bHide) guiO.draw();
            //if(guiC->isVisible()) guiC->draw();
            
        }
            break;
    }
    
    if(debugViewStates.getState(kDEBUGVIEW_SHOWINFO)) debugView->draw();
    
    ofDisableBlendMode();
    
    
}

//--------------------------------------------------------------
void AppController::exit(){
    appModel->save("config", ARCHIVE_BINARY);
   // delete appView;
    delete debugView;
}

//--------------------------------------------------------------
void AppController::keyPressed(ofKeyEventArgs & e){
    
    StateGroup & debugViewStates = appModel->getStateGroup("DebugViewStates");
    StateGroup & appViewStates = appModel->getStateGroup("AppViewStates");
    
//    if(appModel->getGui()->hasKeyboardFocus()) return;
    
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
           // appView->resetCamera();
            break;
        case 'r':
//            gui->getWidgets()[0]->setPosition(ofRandom(-100, 100), ofRandom(-100, 100));
            break;
        case 'h':
//            gui->toggleVisible();
            break;
        case 'j':
//            guiC->toggleVisible();
            break;
        case 'k':
//            bHide ^= true;
            break;
        case ' ':
        {
            bizzy = !bizzy;
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
//        appModel->getGui()->setPosition(e.x, e.y);
    //}
}

//--------------------------------------------------------------
void AppController::mousePressed(ofMouseEventArgs & e){

}

//--------------------------------------------------------------
void AppController::mouseReleased(ofMouseEventArgs & e){
    
}