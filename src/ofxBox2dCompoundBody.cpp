#include "ofxBox2dCompoundBody.h"

ofxBox2dCompoundBody::ofxBox2dCompoundBody()
{
}

void ofxBox2dCompoundBody::setup(b2World * b2dworld, protoBody protoBody)
{
	if (b2dworld == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dCompoundBody :: setup : - must have a valid world -");
		return;
	}
	double scale = 1.0f/ofxBox2d::getScale();
	gpuCachedCompoundBody = protoBody.mesh;
	name = protoBody.name;
	bodyDef.type = protoBody.type;
	b2BodyDef myBodyDef;
	myBodyDef.type = protoBody.type;
	body = b2dworld->CreateBody(&myBodyDef);
	for (size_t i = 0; i < protoBody.fixtures.size(); i++)
	{
		fixture.density = protoBody.fixtures.at(i).density;
		fixture.friction = protoBody.fixtures.at(i).friction;
		fixture.isSensor = protoBody.fixtures.at(i).isSensor;
		fixture.restitution = protoBody.fixtures.at(i).restitution;
		if (protoBody.fixtures.at(i).fixture_type == b2Shape::Type::e_circle)
		{
			for (size_t circleIndex = 0; circleIndex < protoBody.fixtures.at(i).polygons.size(); circleIndex++)
			{
				auto data = protoBody.fixtures.at(i).polygons.at(circleIndex);
				b2CircleShape circleShape;
				circleShape.m_p.Set(data.at(1).x*scale, data.at(1).y*scale);
				circleShape.m_radius = data.at(0).x*scale;
				fixture.shape = &circleShape;
				body->CreateFixture(&fixture);
			}
		}
		else if (protoBody.fixtures.at(i).fixture_type == b2Shape::Type::e_polygon)
		{
			for (size_t polygonIndex = 0; polygonIndex < protoBody.fixtures.at(i).polygons.size(); polygonIndex++)
			{
				auto data = protoBody.fixtures.at(i).polygons.at(polygonIndex);
				b2Vec2 vertices[8];
				b2PolygonShape polygonShape;
				for (size_t pointInPoligon = 0; pointInPoligon < data.size(); pointInPoligon++)
				{
					vertices[pointInPoligon].Set(data.at(pointInPoligon).x*scale, data.at(pointInPoligon).y*scale);
				}
				polygonShape.Set(vertices, data.size());
				fixture.shape = &polygonShape;
				body->CreateFixture(&fixture);
			}
		}
	}
}

void ofxBox2dCompoundBody::draw()
{
	ofPushMatrix();
	auto p = getPosition();
	
	auto disp = body->GetLocalCenter() * ofxBox2d::getScale();
	ofVec2f translation = getPosition();
	translation.x = translation.x - disp.x;
	translation.y = translation.y - disp.y;
	ofTranslate(translation);
	auto r = getRotation();
	ofRotateDeg(getRotation(), 0, 0, 1);
	//gpuCachedCompoundBody.draw();
	gpuCachedCompoundBody.drawWireframe();
	//gpuCachedCompoundBody.drawVertices();
	ofPopMatrix();
}
