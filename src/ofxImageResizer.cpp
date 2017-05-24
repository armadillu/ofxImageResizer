//
//  ofxImageResizer.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 14/04/2017.
//
//

#include "ofxImageResizer.h"


float ofxImageResizer::busyTime = 0.0;

ofxImageResizer::ofxImageResizer(){

	ofAddListener(ofEvents().update, this, &ofxImageResizer::update);
	maxNumThreads = MAX(1,std::thread::hardware_concurrency() - 1);
	ofLogNotice("ofxImageResizer") << "operations will be limited to " << maxNumThreads << " threads.";
}


void ofxImageResizer::resizeImage(const string & imgSrc,
								  const string & imgDst,
								  ofVec2f targetImgSize,
								  bool overwrite,
								  bool keepOriginalImgAspectRatio,
								  cv::InterpolationFlags scalingMethod,
								  ofImageQualityType quality){

	ResizeJob job = {imgSrc, imgDst, targetImgSize, overwrite, keepOriginalImgAspectRatio, scalingMethod, quality};
//	bool alreadyExists = false;
//	if(!job.overwrite){ //if asked to not overwrite, check if file is there
//		alreadyExists = ofFile::doesFileExist(job.imgDst);
//	}
//	if(!alreadyExists){
	lock();
		pendingJobs.push_back(job);
	unlock();
//	}

}

void ofxImageResizer::update(ofEventArgs &){

	lock();
	int pending = pendingJobs.size();
	unlock();

	if(pending > 0 && !isThreadRunning()){
		startThread();
	}
}


void ofxImageResizer::executeJob(ofxImageResizer::ResizeJob job, bool * finished){

	//float t = ofGetElapsedTimef();
	bool alreadyExists = false;
	if(!job.overwrite){ //if asked to not overwrite, check if file is there
		alreadyExists = ofFile::doesFileExist(job.imgDst);
	}
	if(!alreadyExists){
		ofPixels original;
		bool ok = ofLoadImage(original, job.imgSrc);
		if (ok){
			//original.resize(job.imgSize.x, job.imgSize.y); //leads to ugly results, no matter the interpolation method
			int dstW = job.imgSize.x;
			int dstH = job.imgSize.y;
			if(job.keepOriginalImgAspectRatio){
				ofRectangle orSize = ofRectangle(0,0, original.getWidth(), original.getHeight());
				ofRectangle target = ofRectangle(0,0, dstW, dstH);
				orSize.scaleTo(target);
				dstW = orSize.width;
				dstH = orSize.height;
			}
			ofPixels dst;
			dst.allocate(dstW, dstH, original.getNumChannels());
			ofxCv::resize(original, dst, job.inter); //use opencv for fast & nice resizing
			ofSaveImage(dst, job.imgDst, job.quality); //TODO test opencv saving vs freeimage saving to decide what's best here
		}else{
			ofLogError("ofxImageResizer") << "cant load image for resizing! '" << job.imgSrc << "'";
		}
	}
	//busyTime += ofGetElapsedTimef() - t;
	*finished = true;
}

bool ofxImageResizer::isBusy(){
	int pending = 0;
	lock();
	pending = pendingJobs.size();
	unlock();
	return pending > 0;
};

void ofxImageResizer::threadedFunction(){

	int pending = 0;
	lock();
	pending = pendingJobs.size();
	unlock();

	while(isThreadRunning() && pending > 0){
		//check for threads that are done and cleanup
		for(int i = activeThreads.size() - 1; i >= 0; i--){
			ThreadInfo * t = activeThreads[i];
			if(t->finished){ //thread done!
				delete t->thread;
				delete t;
				activeThreads.erase(activeThreads.begin() + i);
			}
		}

		//see if there are pending jobs
		lock();
		pending = pendingJobs.size();
		unlock();

		while (pending > 0 && activeThreads.size() < maxNumThreads){

			ResizeJob job;
			lock();
			job = pendingJobs.front();
			pendingJobs.erase(pendingJobs.begin());
			pending--;
			unlock();

			bool alreadyExists = false;
			if(!job.overwrite){ //if asked to not overwrite, check if file is there
				alreadyExists = ofFile::doesFileExist(job.imgDst);
			}
			if(!alreadyExists){
				ThreadInfo *ti = new ThreadInfo();
				ti->finished = false;
				ti->thread = new std::thread(&ofxImageResizer::executeJob, job, &ti->finished);
				ti->thread->detach();
				activeThreads.push_back(ti);
			}
		}
	}
	ofLogNotice("ofxImageResizer") << "dispatch thread exiting - done"; 
}


void ofxImageResizer::draw(int x, int y){

	string msg = "ofxImageResizer: ";
	if(activeThreads.size()){
		msg += "busy(" + ofToString(activeThreads.size()) + " threads)";
	}else{
		msg += "idle";
	}
	lock();
	int pending = pendingJobs.size();
	unlock();

	msg += "\nPending Jobs: " + ofToString(pending);
	//msg += "\nTotal Busy Time: " + ofToString(busyTime,2);
	ofDrawBitmapStringHighlight(msg, x, y);
}
