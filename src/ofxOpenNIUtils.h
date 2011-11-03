/*
 *  ofxOpenNIUtils.h
 *  openNiSample007
 *
 *  Created by gameover on 31/10/11.
 *  Copyright 2011 trace media. All rights reserved.
 *
 */

/*
 * ofxOpenNIUtils.h
 *
 *  Created on: 11/10/2011
 *      Author: arturo
 */

#ifndef _H_OFXOPENNIUTILS
#define _H_OFXOPENNIUTILS

#include <XnTypes.h>
#include "ofPoint.h"

void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize);

#define SHOW_RC(rc, what)											\
ofLogNotice(LOG_NAME) << what << "status:" << xnGetStatusString(rc);

inline ofPoint toOf(const XnPoint3D & p){
	return *(ofPoint*)&p;
	/*
	 this is more future safe, but currently unnecessary and slower:
	 return ofPoint(p.X,p.Y,p.Z);
	 */
}

inline XnPoint3D toXn(const ofPoint & p){
	
	return *(XnPoint3D*)&p;
	/*
	 this is more future safe, but currently unnecessary and slower:
	 XnPoint3D r;
	 r.X = p.x;
	 r.Y = p.y;
	 r.Z = p.z;
	 return r;
	 */
}
#endif