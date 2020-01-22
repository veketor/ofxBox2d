#include "ofxBox2dCompoundBody.h"

void ofxBox2dCompoundBody::setup(b2World * b2dworld, protoBody protoBody)
{
	if (b2dworld == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dCompoundBody :: setup : - must have a valid world -");
		return;
	}

	float scale = ofxBox2d::getScale();
	name = "CompoundBody";
	bodyDef.type = protoBody.type;
	b2BodyDef myBodyDef;
	myBodyDef.type = protoBody.type;
	b2Body* body = b2dworld->CreateBody(&myBodyDef);
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
				circleShape.m_p.Set(data.at(1).x, data.at(1).y);
				circleShape.m_radius = data.at(0).x;
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
					vertices[pointInPoligon].Set(data.at(pointInPoligon).x, data.at(pointInPoligon).y);
				}
				polygonShape.Set(vertices, data.size());
				fixture.shape = &polygonShape;
				body->CreateFixture(&fixture);
			}
		}
	}
}
