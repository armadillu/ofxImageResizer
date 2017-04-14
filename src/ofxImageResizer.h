//
//  ofxImageResizer.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 14/04/2017.
//
//

#pragma once
#include "ofMain.h"

class ofxImageResizer{

public:
	
	static ofxImageResizer& one(){
		static ofxImageResizer instance; // Instantiated on first use.
		return instance;
	}

	void resizeImage(const string & imgSrc,
					 const string & imgDst,
					 ofVec2f targetImgSize,
					 bool overwrite,
					 ofInterpolationMethod interpol = OF_INTERPOLATE_BICUBIC
					 );

	void update(ofEventArgs &);
	void draw(int x, int y);

	struct ResizeJob{
		string imgSrc;
		string imgDst;
		ofVec2f imgSize;
		bool overwrite;
		ofInterpolationMethod inter;
	};

protected:


	struct ThreadInfo{
		std::thread * thread;
		bool finished;
	};

	vector<ResizeJob> pendingJobs;
	vector<ThreadInfo*> activeThreads;

	static void executeJob(ofxImageResizer::ResizeJob job, bool * finished);

private:

	ofxImageResizer();
	unsigned maxNumThreads = 1;
	static float busyTime;
};
