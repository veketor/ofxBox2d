#include "ofxBox2dCompoundBody.h"

ofxBox2dCompoundBody::ofxBox2dCompoundBody()
{
	std::cout << "Test";
}

void ofxBox2dCompoundBody::setup(b2World * b2dworld, protoBody protoBody, float scaleFactor)
{
	if (b2dworld == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dCompoundBody :: setup : - must have a valid world -");
		return;
	}
	_scaleFactor = 1.0f;// scaleFactor;
	double scale = scaleFactor/ofxBox2d::getScale();
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
	//return;
	//Mesh creation based on body shapes
	int totalVertexB2D = 0;
	float minX = 1000;
	float maxX = -1000;
	float minY = 1000;
	float maxY = -1000;
	//clear the mesh
	gpuCachedCompoundBody.clear();
	for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
	{
		ofMesh tempMesh;
		if (f->GetType() == b2Shape::e_polygon)
		{
			ofPath path;
			b2PolygonShape *shape = (b2PolygonShape*)f->GetShape();
			int nVertex = shape->GetVertexCount();
			for (int nv = 0; nv < nVertex; nv++)
			{
				const b2Vec2 shapeVertex = shape->GetVertex(nv);
				auto meshVertex = toOf(shapeVertex);
				minX = min(minX, meshVertex.x);
				maxX = max(maxX, meshVertex.x);

				minY = min(minY, meshVertex.y);
				maxY = max(maxY, meshVertex.y);

				int a = 0;
				totalVertexB2D++;
				path.lineTo(ofVec3f(meshVertex.x, meshVertex.y, 0));
			}
			tempMesh = path.getTessellation();
		}
		else if (f->GetType() == b2Shape::e_circle)
		{
			size_t circleResolution = 32;
			b2CircleShape *shape = (b2CircleShape*)f->GetShape();
			ofVec2f pos = toOf(shape->m_p);
			float radius = toOf(shape->m_radius);
			for (size_t i = 0; i < circleResolution; i++)
			{
				float angle = i * TWO_PI / circleResolution;
				float cx = pos.x + radius * cos(angle);
				float cy = pos.y + radius * sin(angle);
				ofDefaultVec3 tempVertex(cx, cy, 0);
				//gpuCachedCompoundBody.addVertex(ofDefaultVec3(cx, cy, 0));
				tempMesh.addVertex(ofDefaultVec3(cx, cy, 0));
				
				if (i>0)
				{
					tempMesh.addIndex(0);
					tempMesh.addIndex(i);
					tempMesh.addIndex(i + 1);
					//gpuCachedCompoundBody.addIndex(0);
					//gpuCachedCompoundBody.addIndex(i);
					//gpuCachedCompoundBody.addIndex(i+1);
				}
			}
		}
		gpuCachedCompoundBody.append(tempMesh);
		f->GetNext();
	}
	totalVertexB2D;
	ofVec2f localCenter = toOf(body->GetLocalCenter());
	auto centroid = gpuCachedCompoundBody.getCentroid();
	ofDefaultVertexType *vertexPointer = gpuCachedCompoundBody.getVerticesPointer();
	for (size_t vertexNumber = 0; vertexNumber < gpuCachedCompoundBody.getNumVertices(); vertexNumber++)
	{
		vertexPointer->x = vertexPointer->x - (localCenter.x);
		vertexPointer->y = vertexPointer->y - (localCenter.y);
		vertexPointer++;
	}
	auto centroid2 = gpuCachedCompoundBody.getCentroid();
}

void ofxBox2dCompoundBody::draw()
{
	if (!isBody()) return;
	float scale = ofxBox2d::getScale();
	ofPushMatrix();
	ofTranslate(getPosition().x, getPosition().y, 0);
	ofRotateDeg(getRotation(), 0, 0, 1);
	ofScale(_scaleFactor);
	gpuCachedCompoundBody.drawWireframe();
	ofPopMatrix();
}
