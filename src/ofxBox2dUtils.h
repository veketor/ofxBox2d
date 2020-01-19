#pragma once
#include "ofMain.h"
#include "Box2D.h"

struct protoFixture
{
	float density;
	float friction;
	float restitution;
	int filter_categoryBits;
	int filter_groupIndex;
	int filter_maskBits;
	bool isSensor;//TODO: implement in loader.
	b2Shape::Type fixture_type;
	std::vector< std::vector<ofVec2f>> polygons;
};

struct protoBody
{
	std::string name;
	ofVec2f anchorPoint;
	std::vector<protoFixture> fixtures;
	b2BodyType type;

	b2Body *create(b2World *world)
	{
		b2BodyDef myBodyDef;
		myBodyDef.type = type;
		b2Body* body = world->CreateBody(&myBodyDef);
		for (size_t i = 0; i < fixtures.size(); i++)
		{
			b2FixtureDef tempFixtureDef;
			tempFixtureDef.density = fixtures.at(i).density;
			tempFixtureDef.friction = fixtures.at(i).friction;
			tempFixtureDef.isSensor = fixtures.at(i).isSensor;
			tempFixtureDef.restitution = fixtures.at(i).restitution;
			//tempFixtureDef.shape
			if (fixtures.at(i).fixture_type == b2Shape::Type::e_circle)
			{
				for (size_t circleIndex = 0; circleIndex < fixtures.at(i).polygons.size(); circleIndex++)
				{
					auto data = fixtures.at(i).polygons.at(circleIndex);
					b2CircleShape circleShape;
					circleShape.m_p.Set(data.at(1).x, data.at(1).y);
					circleShape.m_radius = data.at(0).x;
					tempFixtureDef.shape = &circleShape;
					body->CreateFixture(&tempFixtureDef);
				}
			}
			else if (fixtures.at(i).fixture_type == b2Shape::Type::e_polygon)
			{
				for (size_t polygonIndex = 0; polygonIndex < fixtures.at(i).polygons.size(); polygonIndex++)
				{
					auto data = fixtures.at(i).polygons.at(polygonIndex);
					b2Vec2 vertices[8];
					b2PolygonShape polygonShape;
					for (size_t pointInPoligon = 0; pointInPoligon < data.size(); pointInPoligon++)
					{
						vertices[pointInPoligon].Set(data.at(pointInPoligon).x, data.at(pointInPoligon).y);
					}
					polygonShape.Set(vertices, data.size());
					tempFixtureDef.shape = &polygonShape;
					body->CreateFixture(&tempFixtureDef);
				}
			}

		}
		return body;
	}
};

class QueryCallback : public b2QueryCallback {
	
public:
	
	QueryCallback(const b2Vec2& point) {
		m_point = point;
		m_fixture = NULL;
	}
	
	bool ReportFixture(b2Fixture* fixture) {
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;
				
				// We are done, terminate the query.
				return false;
			}
		}
		
		// Continue the query.
		return true;
	}
	
	b2Vec2 m_point;
	b2Fixture* m_fixture;
};
