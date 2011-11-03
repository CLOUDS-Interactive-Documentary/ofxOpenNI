//#ifndef _H_SINGLETON
//#define _H_SINGLETON
//#include "assert.h"
//#include <cstdlib>
//template <class T>
//class Singleton {
//public:
//	static T* Instance() {
//		if(!m_pInstance) m_pInstance = new T;
//		assert(m_pInstance !=NULL);
//		return m_pInstance;
//	}
//protected:
//	Singleton();
//	~Singleton();
//private:
//	Singleton(Singleton const&);
//	Singleton& operator=(Singleton const&);
//	static T* m_pInstance;
//};
//template <class T> T* Singleton<T>::m_pInstance=NULL;
//#endif

#ifndef	_H_OFXOPENNICONTEXT
#define _H_OFXOPENNICONTEXT

#define MAX_DEVICES 2

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include <XnTypes.h>
#include "ofxOpenNIUtils.h"
#include "ofLog.h"
#include "ofConstants.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofThread.h"

using namespace xn;

class ofxOpenNIContext : public ofThread {
	
public:
	
	ofxOpenNIContext();
	~ofxOpenNIContext();
	
	bool initContext();
	bool addLicence(string sVendor, string sKey);
	void setLogLevel(XnLogSeverity logLevel);
	
	int getNumDevices();
	
	bool getIsContextReady();
	xn::Context& getContext();
	
	xn::Device& getDevice(int deviceID = 0);
	xn::DepthGenerator& getDepthGenerator(int deviceID = 0);
	xn::ImageGenerator& getImageGenerator(int deviceID = 0);
	xn::IRGenerator& getIRGenerator(int deviceID = 0);
	xn::AudioGenerator& getAudioGenerator(int deviceID = 0);
	xn::Player& getPlayer(int deviceID = 0);
	
	static string LOG_NAME;
	
protected:
	
	void threadedFunction();
	
private:
	
	int enumerateAndCreateXnNode(XnProductionNodeType type, ProductionNode *node);
	
	int numDevices;
	bool bIsContextReady;
	
	// generators/nodes
	xn::Context g_Context;

	xn::Device g_Device[MAX_DEVICES];
	xn::DepthGenerator g_Depth[MAX_DEVICES];
	xn::ImageGenerator g_Image[MAX_DEVICES];
	xn::IRGenerator g_IR[MAX_DEVICES];
	xn::UserGenerator g_User[MAX_DEVICES];
	xn::AudioGenerator g_Audio[MAX_DEVICES];
	xn::Player g_Player[MAX_DEVICES];
	
	xn::ProductionNode *g_pPrimary;
	
	// TODO: use vectors -> although makes enumerateAndCreateXnNodes a little different/harder maybe
	//vector<xn::Device> g_Device;
	//vector<xn::DepthGenerator> g_Depth;
	//vector<xn::ImageGenerator> g_Image;
	//vector<xn::IRGenerator> g_IR;
	//vector<xn::UserGenerator> g_User;
	//vector<xn::AudioGenerator> g_Audio;
	//vector<xn::Player> g_Player;
	
};
//typedef Singleton<ofxOpenNIContext> ofxOpenNIContextSingleton;
//static ofxOpenNIContext * openNIContext = ofPtr<ofxOpenNIContext> ofxOpenNIContextSingleton::Instance();
static ofPtr<ofxOpenNIContext> openNIContext = ofPtr<ofxOpenNIContext>(new ofxOpenNIContext()); // not sure this is the right way to do this???
#endif