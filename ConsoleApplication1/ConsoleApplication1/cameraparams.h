#ifndef _CAMERAPARAMS_H
#define _CAMERAPARAMS_H
#include <cv.h>
using namespace cv;

	CvMat* intrinsic = (CvMat*)cvLoad("intrinsic.xml");	
	CvMat* distor = (CvMat*)cvLoad("distortion.xml");

	Mat cameraMatrix = cvarrToMat(intrinsic);
	Mat distortions = cvarrToMat(distor);



#endif


