#ifndef	_H_OFXOPENNI
#define _H_OFXOPENNI

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

class ofxOpenNI : public ofThread {//: public ofxOpenNIBaseContext {
	
public:
	
	enum DepthColoring {
		COLORING_PSYCHEDELIC_SHADES = 0,
		COLORING_PSYCHEDELIC,
		COLORING_RAINBOW,
		COLORING_CYCLIC_RAINBOW,
		COLORING_BLUES,
		COLORING_GREY,
		COLORING_STATUS,
		COLORING_COUNT
	};
	
	ofxOpenNI();
	~ofxOpenNI();
	
	bool setup(bool threaded = true);
	bool setup(string xmlFilePath, bool threaded = true);
	
	bool addDepthGenerator();
	bool addImageGenerator();
	bool addInfraGenerator();
	bool addAudioGenerator();
	bool addUserGenerator();
	bool addPlayerGenerator();
	
	void update();
	
	void drawDepth();
	void drawDepth(float x, float y);
	void drawDepth(float x, float y, float w, float h);
	
	void drawImage();
	void drawImage(float x, float y);
	void drawImage(float x, float y, float w, float h);
	
	float getWidth();
	float getHeight();

	int getNumDevices();
	
	bool isNewFrame();
	
	void setUseTexture(bool useTexture);
	void setDepthColoring(DepthColoring coloring);
	
	bool toggleCalibratedRGBDepth();
	bool enableCalibratedRGBDepth();
	bool disableCalibratedRGBDepth();
	
	ofPixels& getDepthPixels();
	ofShortPixels& getDepthRawPixels();
	ofPixels& getImagePixels();
	
	ofTexture& getDepthTextureReference();
	ofTexture& getimageTextureReference();
	
	ofPoint worldToProjective(const ofPoint & p);
	ofPoint worldToProjective(const XnVector3D & p);
	
	ofPoint projectiveToWorld(const ofPoint & p);
	ofPoint projectiveToWorld(const XnVector3D & p);
	
	ofPoint cameraToWorld(const ofVec2f& c);
	void cameraToWorld(const vector<ofVec2f>& c, vector<ofVec3f>& w);
	
	xn::Device& getDevice();
	xn::DepthGenerator& getDepthGenerator();
	xn::ImageGenerator& getImageGenerator();
	xn::IRGenerator& getIRGenerator();
	xn::AudioGenerator& getAudioGenerator();
	xn::Player& getPlayer();
	
	xn::DepthMetaData& getDepthMetaData();
	xn::ImageMetaData& getImageMetaData();
	xn::IRMetaData& getIRMetaData();
	xn::AudioMetaData& getAudioMetaData();
	
	static string LOG_NAME;
	
protected:
	
	void threadedFunction();
	
private:
	
	bool initContext();
	int initDevice();
	bool addLicence(string sVendor, string sKey);
	void setLogLevel(XnLogSeverity logLevel);
	
	bool addGenerator(bool& g_bNodeIsOn, xn::MapGenerator& g_Node);
	
	//void openCommon();
	//void initConstants();
	void readFrame();
	void generateDepthPixels();
	void generateImagePixels();
	void generateIRPixels();
	void allocateDepthBuffers();
	void allocateDepthRawBuffers();
	void allocateImageBuffers();
	void allocateIRBuffers();
	
	int numDevices;
	
	bool g_bIsDepthOn;
	bool g_bIsImageOn;
	bool g_bIsIROn;
	bool g_bIsAudioOn;
	bool g_bIsPlayerOn;
	bool g_bIsDepthRawOnOption;
	
	bool bIsThreaded;
	bool bIsContextReady;
	bool bUseTexture;
	bool bNewPixels;
	bool bNewFrame;
	
	// depth
	ofTexture depthTexture;
	ofPixels depthPixels[2];
	ofPixels* backDepthPixels;
	ofPixels* currentDepthPixels;
	DepthColoring depthColoring;
	float maxDepth;
	
	// depth raw
	ofShortPixels depthRawPixels[2];
	ofShortPixels* backDepthRawPixels;
	ofShortPixels* currentDepthRawPixels;
	
	// rgb
	ofTexture imageTexture;
	ofPixels imagePixels[2];
	ofPixels* backImagePixels;
	ofPixels* currentImagePixels;
	
	// meta data
	xn::DepthMetaData g_DepthMD;
	xn::ImageMetaData g_ImageMD;
	xn::IRMetaData	g_IrMD;
	xn::AudioMetaData g_AudioMD;
	
	// generators/nodes
	
	xn::MockDepthGenerator mockDepth;
	xn::ProductionNode *g_pPrimary;
	
	// generators/nodes
	xn::Context g_Context;
	xn::Device g_Device;
	xn::DepthGenerator g_Depth;
	xn::ImageGenerator g_Image;
	xn::IRGenerator g_IR;
	xn::UserGenerator g_User;
	xn::AudioGenerator g_Audio;
	xn::Player g_Player;
	
	int instanceID;
	
};

#endif
