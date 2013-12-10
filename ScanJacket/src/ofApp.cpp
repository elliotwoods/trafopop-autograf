#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	auto devices = camera.listDevices();
	for(auto device : devices) {
		cout << device.deviceName << endl;
	}
	camera.setDeviceID(1);
	camera.initGrabber(640, 480);
	
	serial.setup(0, 115200);
	
	this->frameIndex = 0;
	this->lastChange = 0.0f;
	this->hasChange = false;
}

//--------------------------------------------------------------
void ofApp::update(){
	camera.update();
	
	auto time = ofGetElapsedTimef();
	
	if (serial.available() > 0) {
		const int length = 1000;
		unsigned char readbuffer[length];
		auto bytesReceived = serial.readBytes(readbuffer, length);
		if (bytesReceived > 0) {
			this->frameIndex = readbuffer[0];
			this->hasChange = true;
		}
		lastChange = time;
		serial.flush();
	}
	
	if (time - 0.2f > lastChange && this->hasChange) {
		auto & pixels = camera.getPixelsRef();
		capturePreview = pixels;
		capturePreview.update();
		
		while (frameIndex >= frames.size()) {
			frames.push_back(ofPixels());
		}
		
		frames[frameIndex] = pixels;
		lastChange = time;
		this->calc();
	}
}

//--------------------------------------------------------------
void ofApp::calc() {
	int width = camera.getWidth();
	int height = camera.getHeight();
	int size = width * height;
	
	//--
	//find min and max
	//--
	//
	unsigned char * lowest = new unsigned char[size];
	unsigned char * highest = new unsigned char[size];
	memset(lowest, 255, size);
	memset(highest, 0, size);
	
	for(auto & frame : frames) {
		if (!frame.isAllocated()) {
			continue;
		}
		for(int i=0; i<size; i++) {
			
			int value = 0;
			value += frame.getPixels()[i * 3];
			value += frame.getPixels()[i * 3 + 1];
			value += frame.getPixels()[i * 3 + 2];
			value /= 3;
			
			if (value < lowest[i]) {
				lowest[i] = value;
			}
			if (value > highest[i]) {
				highest[i] = value;
			}
		}
	}
	//
	//--
	
	
	//--
	//median and distance
	//--
	//
	median.allocate(width, height, OF_IMAGE_GRAYSCALE);
	for(int i=0; i<size; i++) {
		median.getPixels()[i] = ((int) highest[i] + (int) lowest[i]) / 2;
	}
	median.update();
	
	difference.allocate(width, height, OF_IMAGE_GRAYSCALE);
	for(int i=0; i<size; i++) {
		difference.getPixels()[i] = highest[i] - lowest[i];
	}
	difference.update();
	
	delete[] lowest;
	delete[] highest;
	//
	//--
}

//----------
void ofApp::decode() {
	int width = camera.getWidth();
	int height = camera.getHeight();
	int size = width * height;
	
	//--
	//recover indices in image
	//--
	//
	vector<int> indices(size, 0);
	for(int i = 0; i < size; i++) {
		if (difference.getPixels()[i] > 10) {
			indices[i] = 0;
			for(int bit=0; bit<frames.size(); bit++) {
				if (frames[bit].getPixels()[i * 3 + 1] > median.getPixels()[i]) {
					indices[i] += 1 << bit;
				}
			}
		}
	}
	//
	//--
	
	
	//--
	//find average position of each index
	//--
	//
	centers.resize(NUM);
	for(int i=0; i<NUM; i++) {
		vector<float> finds_x, finds_y;
		int count = 0;
		for(int p=0; p<size; p++) {
			if (indices[p] == i) {
				finds_x.push_back(p % width);
				finds_y.push_back(p / width);
				count++;
			}
		}
		if (count < 4) {
			continue;
		}
		
		sort(finds_x.begin(), finds_x.end());
		sort(finds_y.begin(), finds_y.end());
		
		ofVec2f total;
		for(int iX = count / 4; iX < count * 3 / 4; iX++) {
			total.x += finds_x[iX];
			total.y += finds_y[iX];
		}
		ofVec2f mean = total / (float) (count / 2);
		centers[i] = mean;
	}
	//
	//--
}

//--------------------------------------------------------------
void ofApp::draw(){
	camera.draw(0,0);
	capturePreview.draw(640, 0);
	median.draw(0, 480);
	difference.draw(640, 480);
	
	ofDrawBitmapStringHighlight(ofToString(frameIndex), 10, 30);
	
	for(int i=0; i<centers.size(); i++) {
		ofDrawBitmapStringHighlight(ofToString(i), centers[i]);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'd') {
		this->decode();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
