#include "ofxProtoBodies.h"

bool oxfProtoBodies::loadFromXml(std::string xmlFilePath)
{
	std::vector<protoBody> bodiesInXml;
	ofXml _xml;
	if (!_xml.load(xmlFilePath))
	{
		ofLogError() << "xmlBodyLoader Couldn't load file " << xmlFilePath;
		return false;
	}
	ofXml xmlBodyDef = _xml.findFirst("bodydef");
	ofXml::Search bodyXML = xmlBodyDef.findFirst("bodies").find("body");

	float worldRatio = xmlBodyDef.findFirst("metadata").findFirst("ptm_ratio").getDoubleValue();
	for (auto &body : bodyXML)
	{
		protoBody tempBody;
		tempBody.name = body.getAttribute("name").getValue();
		ofXml anchor = body.findFirst("anchorpoint");
		std::string anchorValue = anchor.getValue();
		auto anchorSplited = ofSplitString(anchorValue, ",");
		tempBody.anchorPoint.set(stof(anchorSplited.at(0)), stof(anchorSplited.at(1)));
		std::cout << "Name: " << tempBody.name << " anchor: [" << tempBody.anchorPoint[0] << ", " << tempBody.anchorPoint[1] << "]\n";
		tempBody.type = b2_staticBody;//A body is static unles one of the fixtures has density > 0

		ofXml::Search fixtureInfosXml = body.findFirst("fixtures").find("fixture");
		for (auto &fixtureInfo : fixtureInfosXml)
		{
			protoFixture tempFixture;
			tempFixture.density = fixtureInfo.findFirst("density").getDoubleValue();
			if (tempBody.type == b2_staticBody && tempFixture.density > 0)
			{
				tempBody.type = b2_dynamicBody;
			}
			tempFixture.friction = fixtureInfo.findFirst("friction").getFloatValue();
			tempFixture.restitution = fixtureInfo.findFirst("restitution").getFloatValue();
			tempFixture.filter_categoryBits = fixtureInfo.findFirst("filter_categoryBits").getIntValue();
			tempFixture.filter_groupIndex = fixtureInfo.findFirst("filter_groupIndex").getIntValue();
			tempFixture.filter_maskBits = fixtureInfo.findFirst("filter_maskBits").getIntValue();
			tempFixture.isSensor = false;//get value from xml.
			//std::cout << "\tdensity: " << density << " friction: " << friction << " restitution; " << restitution << "\n";
			std::string fixtureInfoType = fixtureInfo.findFirst("fixture_type").getValue();
			if (fixtureInfoType == "CIRCLE")
			{	
				std::cout << "\t\t[CIRCLE]\n";
				tempFixture.fixture_type = b2Shape::e_circle;
				ofXml circleXml = fixtureInfo.findFirst("circle");
				float circleRadious = circleXml.getAttribute("r").getFloatValue();
				float circleX = circleXml.getAttribute("x").getFloatValue();
				float circleY = circleXml.getAttribute("y").getFloatValue();
				ofVec2f rV(circleRadious, circleRadious);
				ofVec2f pV(circleX, circleY);
				std::vector<ofVec2f> tempPoints;
				tempPoints.push_back(rV);
				tempPoints.push_back(pV);
				tempFixture.polygons.push_back(tempPoints);
				ofMesh tempMesh;
				tempMesh.enableIndices();
				tempMesh.setMode(OF_PRIMITIVE_LINE_LOOP);
				tempMesh.addVertex(ofDefaultVec3(circleX, circleY, 0));
				float cx, cy;
				for (size_t i = 0; i < circleResolution; i++)
				{
					float angle = i * TWO_PI / circleResolution;
					cx = circleX + circleRadious * cos(angle);
					cy = circleY + circleRadious * sin(angle);

					ofDefaultVec3 tempVertex(cx, cy, 0);					
					//tempMesh.addVertex(tempVertex);
					tempMesh.addVertex(ofDefaultVec3(cx, cy, 0));

					if (i>0)
					{
						tempMesh.addIndex(0);
						tempMesh.addIndex(i);
						tempMesh.addIndex(i+1);
					}
				}
				tempMesh.addIndex(0);
				tempMesh.addIndex(1);
				tempMesh.addIndex(circleResolution);
				
				tempBody.mesh.append(tempMesh);
			}
			else if (fixtureInfoType == "POLYGON")
			{
				std::cout << "\t\t[POLYGON]\n";
				tempFixture.fixture_type = b2Shape::e_polygon;
				ofXml::Search polygonsXml = fixtureInfo.findFirst("polygons").find("polygon");
				ofMesh tempMesh;
				tempMesh.enableIndices();
				tempMesh.setMode(OF_PRIMITIVE_LINE_LOOP);

				for (auto &polygon : polygonsXml)
				{
					std::string polygonStr = polygon.getValue();
					std::vector<std::string> polStr = ofSplitString(polygonStr, "  ,  ");
					std::cout << "\t\t\t[points][" << polStr.size() << "]" << polygonStr << "\n";
					std::vector<ofVec2f> tempPointsVector;
					for (size_t i = 0; i < polStr.size(); i++)
					{
						ofVec2f tempPoint;
						std::vector<std::string> tempValues = ofSplitString(polStr.at(i), ", ");
						try
						{
							tempPoint.set(std::stof(tempValues.at(0)), stof(tempValues.at(1)));
							tempMesh.addVertex(ofDefaultVec3(tempPoint.x, tempPoint.y, 0));
							//tempMesh.addIndex(i);
							std::cout << "\t\t\t Point at: (" << tempPoint.x << ", " << tempPoint.y << ")\n";
						}
						catch (const std::invalid_argument& ia) {
							std::cerr << "Invalid argument at Polygon Fixture in " << tempBody.name << ": " << ia.what() << '\n';
						}
						tempPointsVector.push_back(tempPoint);
					}

					int lastValidIndex = 0;
					for (size_t gIndex = 0; gIndex < tempMesh.getNumVertices()/3; gIndex++)
					{
						tempMesh.addIndex(3* gIndex);
						tempMesh.addIndex(3 * gIndex + 1);
						lastValidIndex = 3 * gIndex + 2;
						tempMesh.addIndex(lastValidIndex);
					}
					size_t unwindingNumVertex = tempMesh.getNumIndices() - lastValidIndex;
					if (unwindingNumVertex == 1)
					{						
						tempMesh.addIndex(unwindingNumVertex + 1);
						tempMesh.addIndex(unwindingNumVertex + 2);
						tempMesh.addIndex(0);
					}
					else if (unwindingNumVertex == 2)
					{
						tempMesh.addIndex(unwindingNumVertex);
						tempMesh.addIndex(unwindingNumVertex + 1);
						tempMesh.addIndex(unwindingNumVertex + 2);
					}
					tempFixture.polygons.push_back(tempPointsVector);
				}
				tempBody.mesh.append(tempMesh);
			}
			tempBody.fixtures.push_back(tempFixture);
		}
		bodiesInXml.push_back(tempBody);
		_bodyProtoMap.insert({ tempBody.name, tempBody });
	}
	return (bodiesInXml.size()>0);
}

std::vector<protoBody> oxfProtoBodies::getProtoBodiesFromXml(std::string xmlFilePath)
{
	std::vector<protoBody> bodiesInXml;
	ofXml _xml;
	if (!_xml.load(xmlFilePath))
	{
		ofLogError() << "xmlBodyLoader Couldn't load file " << xmlFilePath;
		return bodiesInXml;
	}
	ofXml xmlBodyDef = _xml.findFirst("bodydef");
	ofXml::Search bodyXML = xmlBodyDef.findFirst("bodies").find("body");

	float worldRatio = xmlBodyDef.findFirst("metadata").findFirst("ptm_ratio").getDoubleValue();
	for (auto &body : bodyXML)
	{
		protoBody tempBody;
		tempBody.name = body.getAttribute("name").getValue();
		ofXml anchor = body.findFirst("anchorpoint");
		std::string anchorValue = anchor.getValue();
		auto anchorSplited = ofSplitString(anchorValue, ",");
		tempBody.anchorPoint.set(stof(anchorSplited.at(0)), stof(anchorSplited.at(1)));
		std::cout << "Name: " << tempBody.name << " anchor: [" << tempBody.anchorPoint[0] << ", " << tempBody.anchorPoint[1] << "]\n";
		tempBody.type = b2_staticBody;//A body is static unles one of the fixtures has density > 0

		ofXml::Search fixtureInfosXml = body.findFirst("fixtures").find("fixture");
		for (auto &fixtureInfo : fixtureInfosXml)
		{
			protoFixture tempFixture;
			tempFixture.density = fixtureInfo.findFirst("density").getDoubleValue();
			if (tempBody.type == b2_staticBody && tempFixture.density > 0)
			{
				tempBody.type = b2_dynamicBody;
			}
			tempFixture.friction = fixtureInfo.findFirst("friction").getFloatValue();
			tempFixture.restitution = fixtureInfo.findFirst("restitution").getFloatValue();
			tempFixture.filter_categoryBits = fixtureInfo.findFirst("filter_categoryBits").getIntValue();
			tempFixture.filter_groupIndex = fixtureInfo.findFirst("filter_groupIndex").getIntValue();
			tempFixture.filter_maskBits = fixtureInfo.findFirst("filter_maskBits").getIntValue();
			//std::cout << "\tdensity: " << density << " friction: " << friction << " restitution; " << restitution << "\n";
			std::string fixtureInfoType = fixtureInfo.findFirst("fixture_type").getValue();

			if (fixtureInfoType == "CIRCLE")
			{
				std::cout << "\t\t[CIRCLE]\n";
				tempFixture.fixture_type = b2Shape::e_circle;
				ofXml circleXml = fixtureInfo.findFirst("circle");
				float circleRadious = circleXml.getAttribute("r").getFloatValue();
				float circleX = circleXml.getAttribute("x").getFloatValue();
				float circleY = circleXml.getAttribute("y").getFloatValue();
				ofVec2f rV(circleRadious, circleRadious);
				ofVec2f pV(circleX, circleY);
				std::vector<ofVec2f> tempPoints;
				tempPoints.push_back(rV);
				tempPoints.push_back(pV);
				tempFixture.polygons.push_back(tempPoints);

			}
			else if (fixtureInfoType == "POLYGON")
			{
				std::cout << "\t\t[POLYGON]\n";
				tempFixture.fixture_type = b2Shape::e_polygon;
				ofXml::Search polygonsXml = fixtureInfo.findFirst("polygons").find("polygon");
				for (auto &polygon : polygonsXml)
				{
					std::string polygonStr = polygon.getValue();
					std::vector<std::string> polStr = ofSplitString(polygonStr, "  ,  ");
					std::cout << "\t\t\t[points][" << polStr.size() << "]" << polygonStr << "\n";
					std::vector<ofVec2f> tempPointsVector;
					for (size_t i = 0; i < polStr.size(); i++)
					{
						ofVec2f tempPoint;
						std::vector<std::string> tempValues = ofSplitString(polStr.at(i), ", ");
						try
						{
							tempPoint.set(std::stof(tempValues.at(0)), stof(tempValues.at(1)));
							std::cout << "\t\t\t Point at: (" << tempPoint.x << ", " << tempPoint.y << ")\n";
						}
						catch (const std::invalid_argument& ia) {
							std::cerr << "Invalid argument at Polygon Fixture in " << tempBody.name << ": " << ia.what() << '\n';
						}
						tempPointsVector.push_back(tempPoint);
					}
					tempFixture.polygons.push_back(tempPointsVector);
				}
			}
			tempBody.fixtures.push_back(tempFixture);
		}
		bodiesInXml.push_back(tempBody);
	}
	return bodiesInXml;
}

protoBody oxfProtoBodies::getProtoBody(std::string nameInXml, std::string newName)
{
	protoBody tempBody;
	std::map<std::string, protoBody>::iterator it;
	it = _bodyProtoMap.find(nameInXml);
	if (it != _bodyProtoMap.end())
	{
		tempBody = it->second;
		if (newName != "")
		{
			tempBody.name = newName;
		}
	}
	else
	{
		tempBody.type = b2_staticBody;
	}
	return tempBody;
}
