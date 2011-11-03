/*
 *  ofxOpenNI.cpp
 *  openNiSample007
 *
 *  Created by gameover on 27/10/11.
 *  Copyright 2011 trace media. All rights reserved.
 *
 */

#include "ofxOpenNI.h"

static xn::Context g_GlobalDeviceContext;
static bool g_bIsGlobalDeviceContextReady = false;
string ofxOpenNI::LOG_NAME = "ofxOpenNI";
static bool rainbowPalletInit = false;
static int instanceCount = -1;

XnUInt8 PalletIntsR [256] = {0};
XnUInt8 PalletIntsG [256] = {0};
XnUInt8 PalletIntsB [256] = {0};

//--------------------------------------------------------------
static void CreateRainbowPallet() {
	if(rainbowPalletInit) return;
	
	unsigned char r, g, b;
	for (int i=1; i<255; i++) {
		if (i<=29) {
			r = (unsigned char)(129.36-i*4.36);
			g = 0;
			b = (unsigned char)255;
		}
		else if (i<=86) {
			r = 0;
			g = (unsigned char)(-133.54+i*4.52);
			b = (unsigned char)255;
		}
		else if (i<=141) {
			r = 0;
			g = (unsigned char)255;
			b = (unsigned char)(665.83-i*4.72);
		}
		else if (i<=199) {
			r = (unsigned char)(-635.26+i*4.47);
			g = (unsigned char)255;
			b = 0;
		}
		else {
			r = (unsigned char)255;
			g = (unsigned char)(1166.81-i*4.57);
			b = 0;
		}
		PalletIntsR[i] = r;
		PalletIntsG[i] = g;
		PalletIntsB[i] = b;
	}
	rainbowPalletInit = true;
}

//--------------------------------------------------------------
ofxOpenNI::ofxOpenNI() {
	
	instanceCount++;
	instanceID = instanceCount;
	
	CreateRainbowPallet();
	
	numDevices = 0;
	bIsThreaded = false;
	bIsContextReady = false;
	
	g_bIsDepthOn = false;
	g_bIsDepthRawOnOption = false;
	g_bIsImageOn = false;
	g_bIsIROn = false;
	g_bIsAudioOn = false;
	g_bIsPlayerOn = false;
	
	depthColoring = COLORING_RAINBOW;
	
	bUseTexture = true;
	bNewPixels = false;
	bNewFrame = false;
	
	g_pPrimary = NULL;
}

//--------------------------------------------------------------
ofxOpenNI::~ofxOpenNI() {
	if (bIsThreaded) stopThread();
	g_Depth.Release();
	g_Image.Release();
	g_IR.Release();
	g_User.Release();
	g_Audio.Release();
	g_Context.Release();
}

//--------------------------------------------------------------
bool ofxOpenNI::setup(bool threaded) {
	
	ofLogWarning(LOG_NAME) << "THIS IS EXPERIMENTAL CODE!! TRYING TO GET MULTIPLE CONTEXTS WORKING (IE., ONE PER DEVICE). IT DOESN'T WORK - NOT SURE WHY :-(";
	
	XnStatus nRetVal;
	
//	nRetVal = g_Context.StopGeneratingAll();
//	ofLogVerbose(LOG_NAME) << "Stop ALL generators:" << xnGetStatusString(nRetVal);
	
	bIsThreaded = threaded;
	
	if (!bIsContextReady) {
		if (!initContext()) {
			ofLogError(LOG_NAME) << "Context could not be intitilised";
			return false;
		}
	}
	
	initDevice();
	
	if (numDevices > 0) {
		
		ofLogNotice(LOG_NAME) << "Found" << numDevices << "devices.";
		
//		nRetVal = g_Context.StartGeneratingAll();
//		ofLogVerbose(LOG_NAME) << "Start ALL generators:" << xnGetStatusString(nRetVal);
		
		if (bIsThreaded) startThread(false, false);
		
		
		
		return true;
	} else {
		ofLogNotice(LOG_NAME) << "No Devices found!"; //Setting up players...
//		numDevices = enumerateAndCreate(XN_NODE_TYPE_PLAYER, g_Player);
//		if (numDevices > 0) {
//			if (bIsThreaded) startThread(false, false);
//			nRetVal = g_Context.StartGeneratingAll();
//			ofLogVerbose(LOG_NAME) << "Start ALL generators:" << xnGetStatusString(nRetVal);
//			
//			return true;
//		} else {
//			ofLogError(LOG_NAME) << "Could not setup Players!";
//			return false;
//		}
	}
	
}

//--------------------------------------------------------------
bool ofxOpenNI::initContext() {
	
	XnStatus nRetVal;
	nRetVal = g_Context.Init();
	
	ofLogVerbose(LOG_NAME) << "OpenNI Context initilized:" << xnGetStatusString(nRetVal);
	
	if (nRetVal == XN_STATUS_OK) {
		bIsContextReady = true;
	} else {
		bIsContextReady =  false;
	}
	
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNI::initDevice() {
	
	NodeInfoList list;
	EnumerationErrors errors;
	XnStatus nRetVal;
	int nodeIndex;
	
	if (!g_bIsGlobalDeviceContextReady) {
		nRetVal = g_GlobalDeviceContext.Init();
		ofLogNotice(LOG_NAME) << "Initilising Global xn::Context" << xnGetStatusString(nRetVal);
		if (nRetVal == XN_STATUS_OK) g_bIsGlobalDeviceContextReady = true;
	}
	
	nRetVal = g_GlobalDeviceContext.EnumerateProductionTrees(XN_NODE_TYPE_DEVICE, NULL, list, &errors);
	
	nodeIndex = 0;
	for (NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it, ++nodeIndex) {
		if (nodeIndex == instanceID) {
			NodeInfo nInfo = *it;
			nRetVal = g_Context.CreateProductionTree(nInfo);
			ofLogNotice(LOG_NAME) << "Create device:" << xnGetStatusString(nRetVal) << "id:" << nodeIndex << nInfo.GetCreationInfo();
		}
		
	}
	
	//	nRetVal = g_GlobalDeviceContext.EnumerateProductionTrees(XN_NODE_TYPE_IR, NULL, list, &errors);
	//	ofLogNotice(LOG_NAME) << "Enumerate ir nodes:" << xnGetStatusString(nRetVal);
	//	nRetVal = g_GlobalDeviceContext.EnumerateProductionTrees(XN_NODE_TYPE_AUDIO, NULL, list, &errors);
	//	ofLogNotice(LOG_NAME) << "Enumerate audio nodes:" << xnGetStatusString(nRetVal);
	//	nRetVal = g_GlobalDeviceContext.EnumerateProductionTrees(XN_NODE_TYPE_USER, NULL, list, &errors);
	//	ofLogNotice(LOG_NAME) << "Enumerate user nodes:" << xnGetStatusString(nRetVal);
	//	nRetVal = g_GlobalDeviceContext.EnumerateProductionTrees(XN_NODE_TYPE_PLAYER, NULL, list, &errors);
	//	ofLogNotice(LOG_NAME) << "Enumerate player nodes:" << xnGetStatusString(nRetVal);
	
	numDevices = nodeIndex;
	return numDevices;
}

//--------------------------------------------------------------
bool ofxOpenNI::addDepthGenerator() {
	NodeInfoList list;
	EnumerationErrors errors;
	XnStatus nRetVal;
	if (!bIsContextReady) {
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
		g_bIsDepthOn = false;
	}
	nRetVal = g_GlobalDeviceContext.EnumerateProductionTrees(XN_NODE_TYPE_DEPTH, NULL, list, &errors);
	ofLogNotice(LOG_NAME) << "Enumerate depth nodes:" << xnGetStatusString(nRetVal);
	if (!g_bIsDepthOn && !g_Depth.IsValid()) {
		nRetVal = g_Depth.Create(g_Context);
		ofLogNotice(LOG_NAME) << g_Depth.GetName() << "generator create:" << xnGetStatusString(nRetVal);
		nRetVal = g_Depth.StartGenerating();
		ofLogNotice(LOG_NAME) << g_Image.GetName() << "generator start:" << xnGetStatusString(nRetVal);
		if (nRetVal == XN_STATUS_OK) {
			g_bIsDepthOn = true;
		} else {
			g_bIsDepthOn = false;
		}
	} else {
		ofLogError(LOG_NAME) << g_Depth.GetName() << "generator already exists!";
		//g_bIsDepthOn = false; // or should this be true?
	}
	
	
	if (g_bIsDepthOn){
		allocateDepthBuffers();
	}
	
	return g_bIsDepthOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addImageGenerator() {
	NodeInfoList list;
	EnumerationErrors errors;
	XnStatus nRetVal;
	if (!bIsContextReady) {
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
		g_bIsImageOn = false;
	}
	nRetVal = g_GlobalDeviceContext.EnumerateProductionTrees(XN_NODE_TYPE_IMAGE, NULL, list, &errors);
	ofLogNotice(LOG_NAME) << "Enumerate image nodes:" << xnGetStatusString(nRetVal);
	if (!g_bIsImageOn && !g_Image.IsValid()) {
		nRetVal = g_Image.Create(g_Context);
		ofLogNotice(LOG_NAME) << g_Image.GetName() << "generator create:" << xnGetStatusString(nRetVal);
		nRetVal = g_Image.StartGenerating();
		ofLogNotice(LOG_NAME) << g_Image.GetName() << "generator start:" << xnGetStatusString(nRetVal);
		if (nRetVal == XN_STATUS_OK) {
			g_bIsImageOn = true;
		} else {
			g_bIsImageOn = false;
		}
	} else {
		ofLogError(LOG_NAME) << g_Image.GetName() << "generator already exists!";
		//g_bIsImageOn = false; // or should this be true?
	}
	
	
	if (g_bIsImageOn){
		allocateImageBuffers();
	}
	
	return g_bIsImageOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addInfraGenerator() {
	XnStatus nRetVal;
	if (!bIsContextReady) {
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
		g_bIsIROn = false;
	}
	if (!g_bIsIROn && !g_IR.IsValid()) {
		nRetVal = g_IR.Create(g_Context);
		ofLogNotice(LOG_NAME) << g_IR.GetName() << "generator create:" << xnGetStatusString(nRetVal);
		nRetVal = g_IR.StartGenerating();
		ofLogNotice(LOG_NAME) << g_Image.GetName() << "generator start:" << xnGetStatusString(nRetVal);
		if (nRetVal == XN_STATUS_OK) {
			g_bIsIROn = true;
		} else {
			g_bIsIROn = false;
		}
	} else {
		ofLogError(LOG_NAME) << g_Image.GetName() << "generator already exists!";
		//g_bIsIROn = false; // or should this be true?
	}
	
	
	if (g_bIsIROn){
		allocateIRBuffers();
	}
	
	return g_bIsIROn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addAudioGenerator() {
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}

//--------------------------------------------------------------
bool ofxOpenNI::addUserGenerator() {
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}

//--------------------------------------------------------------
bool ofxOpenNI::addPlayerGenerator() {
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}


//--------------------------------------------------------------
void ofxOpenNI::allocateDepthBuffers(){
	if(g_bIsDepthOn){
		// make new map mode -> default to 640 x 480 @ 30fps
		// TODO: shift this to a setSize or make part of int/setup request
		XnMapOutputMode mapMode;
		XnStatus nRetVal;
		mapMode.nXRes = XN_VGA_X_RES;
		mapMode.nYRes = XN_VGA_Y_RES;
		mapMode.nFPS  = 30;
		nRetVal = g_Depth.SetMapOutputMode(mapMode);
		maxDepth = g_Depth.GetDeviceMaxDepth();
		ofLogVerbose(LOG_NAME) << "Setting depth resolution:" << xnGetStatusString(nRetVal) << mapMode.nXRes << mapMode.nYRes << maxDepth << mapMode.nFPS;
		depthPixels[0].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_COLOR_ALPHA);
		depthPixels[1].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_COLOR_ALPHA);
		currentDepthPixels = &depthPixels[0];
		backDepthPixels = &depthPixels[1];
		if(bUseTexture) depthTexture.allocate(mapMode.nXRes, mapMode.nYRes, GL_RGBA);
	}
}

//--------------------------------------------------------------
void ofxOpenNI::allocateDepthRawBuffers(){
	if(g_bIsDepthRawOnOption){
		// make new map mode -> default to 640 x 480 @ 30fps
		// TODO: shift this to a setSize or make part of int/setup request
		XnMapOutputMode mapMode;
		XnStatus nRetVal;
		mapMode.nXRes = XN_VGA_X_RES;
		mapMode.nYRes = XN_VGA_Y_RES;
		mapMode.nFPS  = 30;
		nRetVal = g_Depth.SetMapOutputMode(mapMode);
		maxDepth = g_Depth.GetDeviceMaxDepth();
		ofLogVerbose(LOG_NAME) << "Setting depth resolution:" << xnGetStatusString(nRetVal) << mapMode.nXRes << mapMode.nYRes << maxDepth << mapMode.nFPS;
		depthRawPixels[0].allocate(mapMode.nXRes, mapMode.nYRes, OF_PIXELS_MONO);
		depthRawPixels[1].allocate(mapMode.nXRes, mapMode.nYRes, OF_PIXELS_MONO);
		currentDepthRawPixels = &depthRawPixels[0];
		backDepthRawPixels = &depthRawPixels[1];
	}
}

//--------------------------------------------------------------
void ofxOpenNI::allocateImageBuffers(){
	if(g_bIsImageOn){
		// make new map mode -> default to 640 x 480 @ 30fps
		// TODO: shift this to a setSize or make part of int/setup request
		XnMapOutputMode mapMode;
		XnStatus nRetVal;
		mapMode.nXRes = XN_VGA_X_RES;
		mapMode.nYRes = XN_VGA_Y_RES;
		mapMode.nFPS  = 30;
		nRetVal = g_Image.SetMapOutputMode(mapMode);
		ofLogVerbose(LOG_NAME) << "Setting image resolution:" << xnGetStatusString(nRetVal) << mapMode.nXRes << mapMode.nYRes << mapMode.nFPS;
		imagePixels[0].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_COLOR);
		imagePixels[1].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_COLOR);
		currentImagePixels = &imagePixels[0];
		backImagePixels = &imagePixels[1];
		if(bUseTexture) imageTexture.allocate(mapMode.nXRes, mapMode.nYRes, GL_RGB);
	}
}

//--------------------------------------------------------------
void ofxOpenNI::allocateIRBuffers(){
	if(g_bIsIROn){
		// make new map mode -> default to 640 x 480 @ 30fps
		// TODO: shift this to a setSize or make part of int/setup request
		XnMapOutputMode mapMode;
		XnStatus nRetVal;
		mapMode.nXRes = XN_VGA_X_RES;
		mapMode.nYRes = XN_VGA_Y_RES;
		mapMode.nFPS  = 30;
		nRetVal = g_IR.SetMapOutputMode(mapMode);
		ofLogVerbose(LOG_NAME) << "Setting ir resolution:" << xnGetStatusString(nRetVal) << mapMode.nXRes << mapMode.nYRes << mapMode.nFPS;
		imagePixels[0].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_GRAYSCALE);
		imagePixels[1].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_GRAYSCALE);
		currentImagePixels = &imagePixels[0];
		backImagePixels = &imagePixels[1];
		if(bUseTexture) imageTexture.allocate(mapMode.nXRes, mapMode.nYRes, GL_LUMINANCE);
	}
}

//--------------------------------------------------------------
bool ofxOpenNI::setup(string xmlFilePath, bool threaded) {
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}

//--------------------------------------------------------------
void ofxOpenNI::drawDepth() {
	if (bUseTexture) drawDepth(0, 0, g_DepthMD.XRes(), g_DepthMD.YRes());
}

//--------------------------------------------------------------
void ofxOpenNI::drawDepth(float x, float y) {
	if (bUseTexture) drawDepth(x, y, g_DepthMD.XRes(), g_DepthMD.YRes());
}

//--------------------------------------------------------------
void ofxOpenNI::drawDepth(float x, float y, float w, float h) {
	if (bUseTexture) depthTexture.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxOpenNI::drawImage() {
	if (bUseTexture) drawImage(0, 0, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawImage(float x, float y) {
	if (bUseTexture) drawImage(x, y, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawImage(float x, float y, float w, float h) {
	if (bUseTexture) imageTexture.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxOpenNI::setDepthColoring(DepthColoring coloring){
	depthColoring = coloring;
}

//--------------------------------------------------------------
void ofxOpenNI::readFrame(){
	
	if (g_Depth.IsValid() && g_bIsDepthOn){
		g_Depth.GetMetaData(g_DepthMD);
		//generateDepthPixels();
	}
	
	if (g_Image.IsValid() && g_bIsImageOn){
		g_Image.GetMetaData(g_ImageMD);
		//generateImagePixels();
	}
	
	if (g_IR.IsValid() && g_bIsIROn){
		g_IR.GetMetaData(g_IrMD);
		//generateIRPixels();
	}
	
	if (g_Audio.IsValid() && g_bIsAudioOn){
		g_Audio.GetMetaData(g_AudioMD);
	}
	
	//if (bIsThreaded) lock();
	
	if (g_bIsDepthOn) generateDepthPixels();
	if (g_bIsImageOn) generateImagePixels();
	if (g_bIsIROn) generateIRPixels();
	
//	if(g_bIsDepthOn){
//		swap(backDepthPixels, currentDepthPixels);
//		if (g_bIsDepthRawOnOption) {
//			swap(backDepthRawPixels, currentDepthRawPixels);
//		}
//	}
//		
//	if(g_bIsImageOn){
//		swap(backImagePixels, currentImagePixels);
//	}
	bNewPixels = true;
	
	//if (bIsThreaded) unlock();
}

//--------------------------------------------------------------
bool ofxOpenNI::isNewFrame(){
	return bNewFrame;
}

//--------------------------------------------------------------
void ofxOpenNI::threadedFunction() {
	while(isThreadRunning()){
		//readFrame();
	}
}

//--------------------------------------------------------------
void ofxOpenNI::update(){
//	if(!bIsThreaded){
//		readFrame();
//	}else{
//		//lock();
//	}
//	
//	if(bNewPixels){
//		if(g_bIsDepthOn&& bUseTexture){
//			depthTexture.loadData(*backDepthPixels);
//		}
//		if((g_bIsImageOn || g_bIsIROn) && bUseTexture){
//			imageTexture.loadData(*backImagePixels);
//		}
//		bNewPixels = false;
//		bNewFrame = true;
//	}
	
	
//	if (bIsThreaded) unlock();
}

//--------------------------------------------------------------
bool ofxOpenNI::toggleCalibratedRGBDepth(){
	
	if (!g_Image.IsValid()) {
		ofLogError(LOG_NAME) << "No Image generator found: cannot register viewport";
		return false;
	}
	
	// Toggle registering view point to image map
	if (g_Depth.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT))
	{
		
		if(g_Depth.GetAlternativeViewPointCap().IsViewPointAs(g_Image)) {
			disableCalibratedRGBDepth();
		} else {
			enableCalibratedRGBDepth();
		}
		
	} else return false;
	
	return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::enableCalibratedRGBDepth(){
	if (!g_Image.IsValid()) {
		ofLogError(LOG_NAME) << "No Image generator found: cannot register viewport";
		return false;
	}
	
	// Register view point to image map
	if(g_Depth.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)){
		
		XnStatus nRetVal = g_Depth.GetAlternativeViewPointCap().SetViewPoint(g_Image);
		ofLogVerbose(LOG_NAME) << "Register viewpoint depth to RGB:" << xnGetStatusString(nRetVal);
		if(nRetVal!=XN_STATUS_OK) return false;
	}else{
		ofLogVerbose(LOG_NAME) << "Alternative viewpoint not supported";
		return false;
	}
	
	return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::disableCalibratedRGBDepth(){
	
	// Unregister view point from (image) any map
	if (g_Depth.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
		XnStatus nRetVal = g_Depth.GetAlternativeViewPointCap().ResetViewPoint();
		ofLogVerbose(LOG_NAME) << "Unregister viewpoint depth to RGB:" << xnGetStatusString(nRetVal);
		if(nRetVal!=XN_STATUS_OK) return false;
	}else{
		ofLogVerbose(LOG_NAME) << "Alternative viewpoint not supported";
		return false;
	}
	
	return true;
}

//--------------------------------------------------------------
void ofxOpenNI::generateIRPixels(){
	const XnUInt8* pImage = (XnUInt8*)g_IrMD.Data();
	backImagePixels->setFromPixels(pImage, g_IrMD.XRes(),g_IrMD.YRes(),OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
void ofxOpenNI::generateImagePixels(){
	const XnUInt8* pImage = g_ImageMD.Data();
	backImagePixels->setFromPixels(pImage, g_ImageMD.XRes(),g_ImageMD.YRes(),OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofxOpenNI::generateDepthPixels(){
	
	// get the pixels
	const XnDepthPixel* depth = g_DepthMD.Data();
	XN_ASSERT(depth);
	
	if (g_DepthMD.FrameID() == 0) return;
	
	// copy raw values
	if (g_bIsDepthRawOnOption) {
		backDepthRawPixels->setFromPixels(depth, g_DepthMD.XRes(), g_DepthMD.YRes(), OF_IMAGE_COLOR);
	}
	
	// copy depth into texture-map
	float max;
	for (XnUInt16 y = g_DepthMD.YOffset(); y < g_DepthMD.YRes() + g_DepthMD.YOffset(); y++) {
		unsigned char * texture = backDepthPixels->getPixels() + y * g_DepthMD.XRes() * 4 + g_DepthMD.XOffset() * 4;
		for (XnUInt16 x = 0; x < g_DepthMD.XRes(); x++, depth++, texture += 4) {
			XnUInt8 red = 0;
			XnUInt8 green = 0;
			XnUInt8 blue = 0;
			XnUInt8 alpha = 255;
			
			XnUInt16 col_index;
			
			switch (depthColoring){
				case COLORING_PSYCHEDELIC_SHADES:
					alpha *= (((XnFloat)(*depth % 10) / 20) + 0.5);
				case COLORING_PSYCHEDELIC:
					switch ((*depth/10) % 10){
						case 0:
							red = 255;
							break;
						case 1:
							green = 255;
							break;
						case 2:
							blue = 255;
							break;
						case 3:
							red = 255;
							green = 255;
							break;
						case 4:
							green = 255;
							blue = 255;
							break;
						case 5:
							red = 255;
							blue = 255;
							break;
						case 6:
							red = 255;
							green = 255;
							blue = 255;
							break;
						case 7:
							red = 127;
							blue = 255;
							break;
						case 8:
							red = 255;
							blue = 127;
							break;
						case 9:
							red = 127;
							green = 255;
							break;
					}
					break;
				case COLORING_RAINBOW:
					col_index = (XnUInt16)(((*depth) / (maxDepth / 256)));
					red = PalletIntsR[col_index];
					green = PalletIntsG[col_index];
					blue = PalletIntsB[col_index];
					break;
				case COLORING_CYCLIC_RAINBOW:
					col_index = (*depth % 256);
					red = PalletIntsR[col_index];
					green = PalletIntsG[col_index];
					blue = PalletIntsB[col_index];
					break;
				case COLORING_BLUES:
					// 3 bytes of depth: black (R0G0B0) >> blue (001) >> cyan (011) >> white (111)
					max = 256+255+255;
					col_index = (XnUInt16)(((*depth) / (maxDepth / max)));
					if ( col_index < 256 )
					{
						blue	= col_index;
						green	= 0;
						red		= 0;
					}
					else if ( col_index < (256+255) )
					{
						blue	= 255;
						green	= (col_index % 256) + 1;
						red		= 0;
					}
					else if ( col_index < (256+255+255) )
					{
						blue	= 255;
						green	= 255;
						red		= (col_index % 256) + 1;
					}
					else
					{
						blue	= 255;
						green	= 255;
						red		= 255;
					}
					break;
				case COLORING_GREY:
					max = 255;	// half depth
				{
					XnUInt8 a = (XnUInt8)(((*depth) / (maxDepth / max)));
					red		= a;
					green	= a;
					blue	= a;
				}
					break;
				case COLORING_STATUS:
					// This is something to use on installations
					// when the end user needs to know if the camera is tracking or not
					// The scene will be painted GREEN if status == true
					// The scene will be painted RED if status == false
					// Usage: declare a global bool status and that's it!
					// I'll keep it commented so you dont have to have a status on every project
#if 0
				{
					extern bool status;
					max = 255;	// half depth
					XnUInt8 a = 255 - (XnUInt8)(((*depth) / (maxDepth / max)));
					red		= ( status ? 0 : a);
					green	= ( status ? a : 0);
					blue	= 0;
				}
#endif
					break;
			}
			
			texture[0] = red;
			texture[1] = green;
			texture[2] = blue;
			
			if (*depth == 0)
				texture[3] = 0;
			else
				texture[3] = alpha;
		}
	}
	
	
}

//--------------------------------------------------------------
int ofxOpenNI::getNumDevices() {
	return numDevices;
}

//--------------------------------------------------------------
xn::Device& ofxOpenNI::getDevice(){
	return g_Device;
}

//--------------------------------------------------------------
xn::DepthGenerator& ofxOpenNI::getDepthGenerator(){
	return g_Depth;
}

//--------------------------------------------------------------
xn::ImageGenerator& ofxOpenNI::getImageGenerator(){
	return g_Image;;
}

//--------------------------------------------------------------
xn::IRGenerator& ofxOpenNI::getIRGenerator(){
	return g_IR;;
}

//--------------------------------------------------------------
xn::AudioGenerator& ofxOpenNI::getAudioGenerator(){
	return g_Audio;;
}

//--------------------------------------------------------------
xn::Player& ofxOpenNI::getPlayer(){
	return g_Player;
}

//--------------------------------------------------------------
xn::DepthMetaData& ofxOpenNI::getDepthMetaData(){
	return g_DepthMD;
}

//--------------------------------------------------------------
xn::ImageMetaData& ofxOpenNI::getImageMetaData(){
	return g_ImageMD;
}

//--------------------------------------------------------------
xn::IRMetaData& ofxOpenNI::getIRMetaData(){
	return g_IrMD;
}

//--------------------------------------------------------------
xn::AudioMetaData& ofxOpenNI::getAudioMetaData(){
	return g_AudioMD;
}

//--------------------------------------------------------------
ofPixels& ofxOpenNI::getDepthPixels(){
	//Poco::ScopedLock<ofMutex> lock(mutex);
	return *currentDepthPixels;
}

//--------------------------------------------------------------
ofShortPixels& ofxOpenNI::getDepthRawPixels(){
	//Poco::ScopedLock<ofMutex> lock(mutex);
	
	if (!g_bIsDepthRawOnOption) {
		ofLogWarning(LOG_NAME) << "g_bIsDepthRawOnOption was disabled, enabling raw pixels";
		g_bIsDepthRawOnOption = true;
	}
	return *currentDepthRawPixels;
}

//--------------------------------------------------------------
ofPixels& ofxOpenNI::getImagePixels(){
	//Poco::ScopedLock<ofMutex> lock(mutex);
	return *currentImagePixels;
}

//--------------------------------------------------------------
ofTexture& ofxOpenNI::getDepthTextureReference(){
	return depthTexture;
}

//--------------------------------------------------------------
ofTexture& ofxOpenNI::getimageTextureReference(){
	return imageTexture;
}

//--------------------------------------------------------------
float ofxOpenNI::getWidth(){
	if(g_bIsDepthOn){
		return g_DepthMD.XRes();
	}else if(g_bIsImageOn){
		return g_ImageMD.XRes();
	}else if(g_bIsIROn){
		return g_IrMD.XRes();
	}else{
		//ofLogWarning(LOG_NAME) << "getWidth() : We haven't yet initialised any generators, so this value returned is returned as 0";
		return 0;
	}
}

//--------------------------------------------------------------
float ofxOpenNI::getHeight(){
	if(g_bIsDepthOn){
		return g_DepthMD.YRes();
	}else if(g_bIsImageOn){
		return g_ImageMD.YRes();
	}else if(g_bIsIROn){
		return g_IrMD.YRes();
	}else{
		//ofLogWarning(LOG_NAME) << "getHeight() : We haven't yet initialised any generators, so this value returned is returned as 0";
		return 0;
	}
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::worldToProjective(const ofPoint& p){
	XnVector3D world = toXn(p);
	return worldToProjective(world);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::worldToProjective(const XnVector3D& p){
	XnVector3D proj;
	g_Depth.ConvertRealWorldToProjective(1,&p,&proj);
	return toOf(proj);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::projectiveToWorld(const ofPoint& p){
	XnVector3D proj = toXn(p);
	return projectiveToWorld(proj);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::projectiveToWorld(const XnVector3D& p){
	XnVector3D world;
	g_Depth.ConvertProjectiveToRealWorld(1,&p,&world);
	return toOf(world);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::cameraToWorld(const ofVec2f& c){
	vector<ofVec2f> vc(1, c);
	vector<ofVec3f> vw(1);
	
	cameraToWorld(vc, vw);
	
	return vw[0];
}

//--------------------------------------------------------------
void ofxOpenNI::cameraToWorld(const vector<ofVec2f>& c, vector<ofVec3f>& w){
	const int nPoints = c.size();
	w.resize(nPoints);
	if (!g_bIsDepthRawOnOption) {
		ofLogError(LOG_NAME) << "ofxOpenNI::cameraToWorld - cannot perform this function if g_bIsDepthRawOnOption is false. You can enabled g_bIsDepthRawOnOption by calling getDepthRawPixels(..).";
		return;
	}
	
	vector<XnPoint3D> projective(nPoints);
	XnPoint3D *out =&projective[0];
	
	//lock();
	const XnDepthPixel* d = currentDepthRawPixels->getPixels();
	unsigned int pixel;
	for (int i=0; i<nPoints; ++i) {
		pixel  = (int)c[i].x + (int)c[i].y * g_DepthMD.XRes();
		if (pixel >= g_DepthMD.XRes() * g_DepthMD.YRes())
			continue;
		
		projective[i].X = c[i].x;
		projective[i].Y = c[i].y;
		projective[i].Z = float(d[pixel]) / 1000.0f;
	}
	//unlock();
	
	g_Depth.ConvertProjectiveToRealWorld(nPoints,&projective[0], (XnPoint3D*)&w[0]);
	
}


//--------------------------------------------------------------
bool ofxOpenNI::addLicence(string sVendor, string sKey) {
	
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
void ofxOpenNI::setLogLevel(XnLogSeverity logLevel) {
	
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = xnLogSetConsoleOutput(true);
	
	ofLogVerbose(LOG_NAME) << "Set log to console:" << xnGetStatusString(nRetVal);
	
	nRetVal = xnLogSetSeverityFilter(logLevel);	// TODO: set different log levels with code; enable and disable functionality
	ofLogVerbose(LOG_NAME) << "Set log level:" << xnGetStatusString(nRetVal) << logLevel;
	
	xnLogSetMaskState(XN_LOG_MASK_ALL, TRUE);
	
}