#ifndef _H_OFXOPENNICONTEXT
#define _H_OFXOPENNICONTEXT

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include "ofMain.h"

using namespace xn;

class ofxOpenNIContext {

public:

	ofxOpenNIContext();
	~ofxOpenNIContext();

	bool setup();
	bool setupUsingRecording(std::string sRecordedFile);
	bool setupUsingXMLFile(std::string sFile = "");

	void update();

	bool toggleRegisterViewport();
	bool registerViewport();
	bool unregisterViewport();

	bool getDepthGenerator(xn::DepthGenerator* depth_generator);
	bool getImageGenerator(xn::ImageGenerator* image_generator);
	bool getIRGenerator(xn::IRGenerator* ir_generator);
	bool getUserGenerator(xn::UserGenerator* user_generator);
	bool getGestureGenerator(xn::GestureGenerator* gesture_generator);
	bool getHandsGenerator(xn::HandsGenerator* hands_generator);
	bool isUsingRecording();

	void enableLogging();

	bool toggleMirror();
	bool setMirror(XnBool mirroring);

	void shutdown();
	bool createXnNode(XnProductionNodeType type, ProductionNode & node, int nodeIndex);

	xn::Context& getXnContext();

private:

	
	bool initContext();
	void addLicense(std::string sVendor, std::string sKey);
	void logErrors(xn::EnumerationErrors& rErrors);

	int enumerateXnNode(XnProductionNodeType type, NodeInfoList & list);
	bool addDeviceNode(int deviceID);

	bool is_using_recording;
	xn::Context context;
	vector<xn::Device> g_Device;

};

#endif
