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
#include "CameraController.h"
#include "AppModel.h"
#include "DebugView.h"

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
    
    //void guiEvent(ofxUIEventArgs &e);
    
protected:
    
	void setFullScreen(bool b);

    ofPoint mouseHit;
    //KeyModifiers keyModifiers;
    
    DebugView * debugView;
	CameraController * cameraController;
    
};

#endif /* defined(__LAFTest__AppController__) */
