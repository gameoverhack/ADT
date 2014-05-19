//
//  AppModel.h
//  LAF
//
//  Created by gameover on 3/01/14.
//
//

#ifndef __H_APPMODEL
#define __H_APPMODEL

#define USE_OPENFRAMEWORKS_TYPES 1
#define USE_OFXUI 1
#define USE_PRORES

#ifndef INFINITY
#define INFINITY std::numeric_limits<float>::infinity()
#endif

#include "BaseModel.h"
#include "AppStates.h"
#include "VectorUtils.h"
#include "FileList.h"
#include "ofxCv.h"
//#include "KeyModifiers.h"

static ofPoint NoOrigin = ofPoint(-INFINITY, -INFINITY);
static ofRectangle NoTarget = ofRectangle(-INFINITY, -INFINITY, 0, 0);

class AppModel : public BaseModel{
    
public:
    
    AppModel(){
        ofxLogNotice() << "Creating AppModel" << endl;
        //BaseModel::BaseModel();
    }
    
    ~AppModel(){
        ofxLogNotice() << "Destroying AppModel" << endl;
        //BaseModel::~BaseModel();
    }
    
    //--------------------------------------------------------------
    void save(string filename, ArchiveType archiveType){
        Serializer.saveClass(filename, (*this), archiveType);
        BaseModel::save(filename + "_props", archiveType);
    }
    
    //--------------------------------------------------------------
    void backup(string filename, ArchiveType archiveType){
        Serializer.saveClass(filename + "_" + ofGetTimestampString() + ".bak", (*this), archiveType);
        BaseModel::save(filename + "_props_" + ofGetTimestampString() + ".bak", archiveType);
    }
    
    //--------------------------------------------------------------
    void load(string filename, ArchiveType archiveType){
        Serializer.loadClass(filename, (*this), archiveType);
        BaseModel::load(filename + "_props", archiveType);
    }
    
protected:
    
    friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){
//        ar & BOOST_SERIALIZATION_NVP(playerModels);
	}
    
};

BOOST_CLASS_VERSION(AppModel, 4)

typedef Singleton<AppModel> AppModelSingleton;					// Global declaration

static AppModel * appModel	= AppModelSingleton::Instance();

#endif
