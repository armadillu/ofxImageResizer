//
//  ofxImageResizer.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 14/04/2017.
//
//

#pragma once
#include "ofMain.h"
#include "ofxCv.h"

class ofxImageResizer : public ofThread{

public:
	
	static ofxImageResizer& one(){
		static ofxImageResizer instance; // Instantiated on first use.
		return instance;
	}

	void resizeImage(const string & imgSrc,
					 const string & imgDst,
					 ofVec2f targetImgSize,
					 bool overwrite,
					 bool keepOriginalImgAspectRatio = true,
					 cv::InterpolationFlags scalingMethod = cv::INTER_AREA,
					 ofImageQualityType quality = OF_IMAGE_QUALITY_HIGH
					 );

	void update(ofEventArgs &);
	void draw(int x, int y);

	void setMaxThreads(unsigned t){maxNumThreads = MAX(1,t);};
	bool isBusy();

	struct ResizeJob{
		string imgSrc;
		string imgDst;
		ofVec2f imgSize;
		bool overwrite;
		bool keepOriginalImgAspectRatio;
		cv::InterpolationFlags inter;
		ofImageQualityType quality;
	};

protected:

	void threadedFunction();

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
