#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);
	ofDirectory::createDirectory("smallImages", true, true);
	ofDirectory dir;
	dir.listDir("images");

	bool overwrite = true; //if file is already there, should we re-write it?

	for(int i = 0; i < dir.size(); i++){
		if (dir.getFile(i).isFile()){
			string img = dir.getPath(i);
			string imgDst = "smallImages/" + dir.getFile(i).getFileName();
			ofVec2f imgSize = ofVec2f(512,512);

			ofxImageResizer::one().resizeImage(img, imgDst, imgSize, overwrite, OF_INTERPOLATE_BICUBIC);
		}
	}
}


void ofApp::update(){

	float dt = 1./60.;

}


void ofApp::draw(){

	ofxImageResizer::one().draw(20, 20);
}

