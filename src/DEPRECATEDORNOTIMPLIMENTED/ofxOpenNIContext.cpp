#include "ofxOpenNIContext.h"

string ofxOpenNIContext::LOG_NAME = "ofxOpenNIContext";

//--------------------------------------------------------------
ofxOpenNIContext::ofxOpenNIContext() {
	bIsContextReady = false;
	//numDevices = 0;
}

//--------------------------------------------------------------
ofxOpenNIContext::~ofxOpenNIContext() {
	g_Context.Release();
}

//--------------------------------------------------------------
bool ofxOpenNIContext::initContext() {
	
	XnStatus nRetVal;
	
	nRetVal = g_Context.Init();
	ofLogVerbose(LOG_NAME) << "OpenNI Context initilized:" << xnGetStatusString(nRetVal);
	
	numDevices = enumerateAndCreate(XN_NODE_TYPE_DEVICE, g_Device);
	
	if (numDevices > 0) {
		enumerateAndCreate(XN_NODE_TYPE_DEPTH, g_Depth);
		enumerateAndCreate(XN_NODE_TYPE_IR, g_IR);
		enumerateAndCreate(XN_NODE_TYPE_IMAGE, g_Image);
		//enumerateAndCreate(XN_NODE_TYPE_USER, g_User);
		//enumerateAndCreate(XN_NODE_TYPE_AUDIO, g_Audio);
	} else {
		ofLogError(LOG_NAME) << "No devices found!!!";
	}

	startThread(false, false);
	
	
	if (nRetVal == XN_STATUS_OK) {
		bIsContextReady = true;
	} else {
		bIsContextReady =  false;
	}
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNIContext::enumerateAndCreate(XnProductionNodeType type, ProductionNode* nodes) {
	
	NodeInfoList list;
	xn::EnumerationErrors errors;
	XnStatus nRetVal;
	
	ofLogVerbose(LOG_NAME) << "Enumerating" << type << "nodes...";
	nRetVal = g_Context.EnumerateProductionTrees(type, NULL, list,&errors);
	ofLogVerbose(LOG_NAME) << "Enumeration:" << xnGetStatusString(nRetVal);
	
	//logErrors(error) TODO do this!
	
	int nodeIndex = 0;
	for (NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it, ++nodeIndex) {
		NodeInfo nInfo = *it;
		
		nInfo.GetInstance(nodes[nodeIndex]);
		XnBool bExists = nodes[nodeIndex].IsValid();
		
		if(!bExists) {
			nRetVal = g_Context.CreateProductionTree(nInfo, nodes[nodeIndex]);
			ofLogVerbose(LOG_NAME) << "Creating node" << xnGetStatusString(nRetVal) << nodeIndex << nodes[nodeIndex].GetName();
		}
		
	}
	return nodeIndex;
}

//--------------------------------------------------------------
void ofxOpenNIContext::threadedFunction(){
	while(isThreadRunning()){
		XnStatus nRetVal = XN_STATUS_OK;
		//lock();
//		if (g_pPrimary != NULL){
//			nRetVal = openNIContext->getContext().WaitOneUpdateAll(*g_pPrimary);
//		}else{
			nRetVal = g_Context.WaitAnyUpdateAll();
//		}
		
		if (nRetVal != XN_STATUS_OK){
			ofLogError(LOG_NAME) << "Error:" << xnGetStatusString(nRetVal);
			return;
		}
		
		
		//unlock();
	}
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addLicence(string sVendor, string sKey) {
	
	XnLicense license = {0};
	XnStatus nRetVal = XN_STATUS_OK;
	bool ok = true;
	
	nRetVal = xnOSStrNCopy(license.strVendor, sVendor.c_str(),sVendor.size(), sizeof(license.strVendor));
	if(nRetVal != XN_STATUS_OK) {
		ofLogError(LOG_NAME) << "Error creating vendor:" << sVendor;
		ok = false;
	}
	
	nRetVal = xnOSStrNCopy(license.strKey, sKey.c_str(), sKey.size(), sizeof(license.strKey));
	if(nRetVal != XN_STATUS_OK) {
		ofLogError(LOG_NAME) << "Error creating keyy:" << sKey;
		ok = false;
	}	
	
	nRetVal = g_Context.AddLicense(license);
	ofLogVerbose(LOG_NAME) << "Adding licence:" << sVendor << sKey << xnGetStatusString(nRetVal);
	
	if(nRetVal != XN_STATUS_OK) ok = false;
	
	return ok;
	
	//xnPrintRegisteredLicenses();
}

//--------------------------------------------------------------
void ofxOpenNIContext::setLogLevel(XnLogSeverity logLevel) {
	
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = xnLogSetConsoleOutput(true);
	
	ofLogVerbose(LOG_NAME) << "Set log to console:" << xnGetStatusString(nRetVal);
	
	nRetVal = xnLogSetSeverityFilter(logLevel);	// TODO: set different log levels with code; enable and disable functionality
	ofLogVerbose(LOG_NAME) << "Set log level:" << xnGetStatusString(nRetVal) << logLevel;
	
	xnLogSetMaskState(XN_LOG_MASK_ALL, TRUE);
	
}

//--------------------------------------------------------------
bool ofxOpenNIContext::getIsReady(){
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNIContext::getNumDevices() {
	return numDevices;
}

//--------------------------------------------------------------
xn::Context& ofxOpenNIContext::getContext(){
	return g_Context;
}

//--------------------------------------------------------------
xn::Device& ofxOpenNIContext::getDevice(int deviceID){
	return g_Device[deviceID];
}

//--------------------------------------------------------------
xn::DepthGenerator& ofxOpenNIContext::getDepthGenerator(int deviceID){
	return g_Depth[deviceID];
}

//--------------------------------------------------------------
xn::ImageGenerator& ofxOpenNIContext::getImageGenerator(int deviceID){
	return g_Image[deviceID];
}

//--------------------------------------------------------------
xn::IRGenerator& ofxOpenNIContext::getIRGenerator(int deviceID){
	return g_IR[deviceID];
}

//--------------------------------------------------------------
xn::AudioGenerator& ofxOpenNIContext::getAudioGenerator(int deviceID){
	return g_Audio[deviceID];
}

//--------------------------------------------------------------
xn::Player& ofxOpenNIContext::getPlayer(int deviceID){
	return g_Player[deviceID];
}