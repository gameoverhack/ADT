//
//  AppController.h
//  LAFTest
//
//  Created by gameover on 9/01/14.
//
//

#ifndef __H_APPCONTROLLER
#define __H_APPCONTROLLER

#include "BaseController.h"
#include "AppModel.h"
#include "DebugView.h"
#include "ofxThreadedVideo.h"

class AppController : public BaseController{
    
public:
    
    AppController();
    ~AppController();
    
    void setup();
    void update();
    void draw();
    
    void exit();
    void restart();
    
    void keyPressed(ofKeyEventArgs & e);
    void keyReleased(ofKeyEventArgs & e);
    
    void mouseMoved(ofMouseEventArgs & e);
    void mouseDragged(ofMouseEventArgs & e);
    void mousePressed(ofMouseEventArgs & e);
    void mouseReleased(ofMouseEventArgs & e);
    
//    void guiEvent(ofxUIEventArgs &e);
    
protected:
    
    
    vector<int> laintVec;
    int laint;
    float lafloat[100];
    
    ofPoint mouseHit;
    //KeyModifiers keyModifiers;
    
//    Gui * gui;
    
//    ofxUISuperCanvas * guiC;
    
//    ofxPanel guiO;
//    ofxFloatSlider ss[200];
//    bool bHide = false;
    
    ofxThreadedVideo vid;
    
    //AppView * appView;
    DebugView * debugView;
   // DeviceController * deviceController;
    
};

#endif /* defined(__LAFTest__AppController__) */
