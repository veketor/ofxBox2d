#pragma once
//#ifndef __OFXPROTOBODIES__
//#define __OFXPROTOBODIES__
#include "ofMain.h"
#include "ofxBox2d.h"
#include "b2Body.h"

class oxfProtoBodies
{
public:
	bool loadFromXml(std::string xmlFilePath);
	std::vector<protoBody> getProtoBodiesFromXml(std::string xmlFilePath);
	protoBody getProtoBody(std::string nameInXml, std::string newName = "");
private:
	std::map<std::string, protoBody> _bodyProtoMap;
};

//#endif