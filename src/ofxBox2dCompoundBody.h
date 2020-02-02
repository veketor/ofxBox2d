#pragma once
#include "ofMain.h"
#include "ofxProtoBodies.h"
#include "ofxBox2dBaseShape.h"

class ofxBox2dCompoundBody : public ofxBox2dBaseShape {
	
private:
	
	int _numFixtures;
	int _numShapes;
	protoBody _original;
	float _scaleFactor;
	
public:
	~ofxBox2dCompoundBody(){};
	//------------------------------------------------
	ofxBox2dCompoundBody();
	
	//------------------------------------------------
	void setup(b2World * b2dworld, protoBody protoBody, float scaleFactor = 1.0f);
	void setScale(float scale)
	{
	};

	virtual void draw();
	
	ofVboMesh gpuCachedCompoundBody;

	//------------------------------------------------
    //void addAttractionPoint(float x, float y, float amt=1);
    //void addAttractionPoint(ofVec2f pt, float amt=1);
	
	//void addRepulsionForce(float x, float y, float amt);
	//void addRepulsionForce(ofVec2f pt, float amt);
};