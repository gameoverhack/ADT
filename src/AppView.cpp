//
//  AppView.cpp
//  LAFTest
//
//  Created by gameover on 9/01/14.
//
//

#include "AppView.h"

//--------------------------------------------------------------
AppView::AppView(){
    
    ofxLogNotice() << "Constructing AppView" << endl;
    
    /******************************************************
     *******                States                  *******
     *****************************************************/

    StateGroup newAppViewStates("AppViewStates", true);
    newAppViewStates.addState(State(kAPPVIEW_NORMAL, "kAPPVIEW_NORMAL"));
    
    appModel->addStateGroup(newAppViewStates);
    
    StateGroup & appViewStates = appModel->getStateGroup("AppViewStates");
    
    appViewStates.setState(kAPPVIEW_NORMAL);

	//appModel->getTextureMutex().lock();
	//appModel->getContourTexture().allocate(1920.0f, 1080.0f, GL_LUMINANCE);
	//appModel->getVideoTexture().allocate(1920.0f, 1080.0f, GL_BGRA);
 //   appModel->getTextureMutex().unlock();

}

//--------------------------------------------------------------
AppView::~AppView(){
    ofxLogNotice() << "Destroying AppView" << endl;
}

//--------------------------------------------------------------
void AppView::update(){
    
    StateGroup & appViewStates = appModel->getStateGroup("AppViewStates");

    begin();
    {
        
        //ofEnableBlendMode(OF_BLENDMODE_SCREEN);
        
		//appModel->getTextureMutex().lock();
		//appModel->getContourTexture().draw(0,0);
		//appModel->getTextureMutex().unlock();

        //ofDisableBlendMode();

    }
    end();
    
	

}
