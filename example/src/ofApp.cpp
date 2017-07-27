#include "ofApp.h"

/*

 Test/bench results for different downscaling methods on a ton of images

 interpolation      |  cpu time  |  quality
--------------------+------------+-------------
 cv::INTER_NEAREST  |    38sec   |   crap
 cv::INTER_LINEAR   |    39sec   |   less crap but still crap
 cv::INTER_CUBIC    |    40sec   |   looks same as linear - moire everywhere
 cv::INTER_AREA     |    42sec   |   amazing! best choice by far
 cv::INTER_LANCZOS4 |    46sec   |   lots of moire

 */

void ofApp::setup(){

	ofBackground(22);
	ofDirectory::createDirectory("smallImages", true, true);
	ofDirectory dir;
	dir.listDir("images");

	bool overwrite = true; //if file is already there, should we re-write it?
	bool keepAspectRatio = true; 	//should we resize blindly to the specified size,
									//or fit the original img size into your specified size?
	ofVec2f imgSize = ofVec2f(512,512);
	cv::InterpolationFlags scalingMethod = cv::INTER_AREA; //resize quality / time
	ofImageQualityType savedImageQuality = OF_IMAGE_QUALITY_MEDIUM;

	ofxImageResizer::one().setMaxThreads(8);

	for(int i = 0; i < dir.size(); i++){

		if (dir.getFile(i).isFile()){
			string img = dir.getPath(i);
			string imgDst = "smallImages/" + dir.getFile(i).getFileName();
			ofxImageResizer::one().resizeImage(img,
											   imgDst,
											   imgSize,
											   overwrite,
											   keepAspectRatio,
											   scalingMethod,
											   savedImageQuality
											   );
		}
	}
}


void ofApp::update(){

	float dt = 1./60.;

}


void ofApp::draw(){

	ofxImageResizer::one().draw(20, 20);
}

