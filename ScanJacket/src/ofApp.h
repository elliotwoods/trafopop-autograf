#pragma once

#include "ofMain.h"

#define NUM 100

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void calc();
	void decode();
	
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofVideoGrabber camera;
	ofSerial serial;
	int frameIndex;
	
	vector<ofPixels> frames;
	ofImage capturePreview;
	ofImage median;
	ofImage difference;
	vector<ofVec2f> centers;

	float lastChange;
	bool hasChange;
};
