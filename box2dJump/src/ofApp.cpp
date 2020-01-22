#include "ofApp.h"
#include <typeinfo>

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(true);
	ofBackgroundHex(0x1ecfd9);// 0xfdefc2);
	box2d.init();
	box2d.enableEvents();   // <-- turn on the event listener
	box2d.setGravity(0, 30);
	box2d.createGround();
	box2d.setFPS(60.0);
	box2d.registerGrabbing();
	// register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);

	auto c = std::make_shared<ofxBox2dCircle>();
	c->setPhysics(1, 0.5, 0.1);
	c->setup(box2d.getWorld(), 100, 100, 20);

	circles.push_back(c);

	npc = make_shared<ofxBox2dRect>();
	npc->setPhysics(0.2, 0.0, 0.9);
	npc->setup(box2d.getWorld(), 550, 100, 64, 32);

	platform = std::make_shared<ofxBox2dRect>();
	platform->setPhysics(0.0, 0.0, 0.0);
	platform->setup(box2d.getWorld(), 500, 300, 250, 16, 0);
	
	bodyUserData* platformUserData = new bodyUserData;
	platformUserData->entityType = 5; //5 is a platform (test)
	platformUserData->stunnedTimeout = 0.0f;
	platform->body->SetUserData(platformUserData);

	//platform->setup(box2d.getWorld(), 100, 100, 20);

	auto rect = make_shared<ofxBox2dRect>();
	rect->setPhysics(0.0, 0.0, 0.9);
	rect->setup(box2d.getWorld(), 150, 300, 128, 32);
	rect->setRotation(10);
	rectangles.push_back(rect);

	auto rectL = make_shared<ofxBox2dRect>();
	rectL->setPhysics(0.0, 0.5, 0.9);
	rectL->setup(box2d.getWorld(), 2, 2, 32, ofGetHeight()*2);
	rectangles.push_back(rectL);

	auto rectR = make_shared<ofxBox2dRect>();
	rectR->setPhysics(0.0, 0.5, 0.9);
	rectR->setup(box2d.getWorld(), ofGetWindowWidth(), 2, 32, ofGetHeight() * 2);
	rectangles.push_back(rectR);

	auto rectT = make_shared<ofxBox2dRect>();
	rectT->setPhysics(0.0, 0.5, 0.9);
	rectT->setup(box2d.getWorld(), 32, 0, ofGetWindowWidth()*2, 32);
	rectangles.push_back(rectT);
		
	prota = make_shared<ofxBox2dRect>();
	prota->setPhysics(10, 0, 0.05);
	prota->setup(box2d.getWorld(), 200, 32, 16, 32);
	prota->setFixedRotation(true);
	bodyUserData* protaUserData = new bodyUserData;
	protaUserData->entityType = 1;
	protaUserData->stunnedTimeout = 0.0f;
	prota->body->SetUserData(protaUserData);
	key = ofxKeyboard::get();
	moveState = MS_STOP;
	auto loaded = bodiesLoader.loadFromXml("test.xml");
	auto pb = bodiesLoader.getProtoBody("coyote", "enemy");
	auto pb0 = bodiesLoader.getProtoBody("coyote");
	auto pb1 = bodiesLoader.getProtoBody("plataforma");
	auto pb2 = bodiesLoader.getProtoBody("logo");
	auto pb3 = bodiesLoader.getProtoBody("compuesto");
	createdBody = pb3.create(box2d.getWorld());
	int a = 0;
	a = 1;
	compoundBody = make_shared<ofxBox2dCompoundBody>();
	compoundBody->setup(box2d.getWorld(), pb);
	auto pos = compoundBody->getPosition();
	auto isBody = compoundBody->isBody();

	a = 2;
}

//--------------------------------------------------------------
void ofApp::update()
{
	box2d.update();
	float lastFrameTime = ofGetLastFrameTime();
	//std::cout << lastFrameTime << "\n";
	if (lastFrameTime > 0.5)
	{
		lastFrameTime = 0.016f;
	}
	moveState = MS_STOP;
	bool stopped = true;

	if (key->isPushing(OF_KEY_RIGHT)) {
		float impulse = b2Max(prota->body->GetLinearVelocity().x + 0.01f, 0.5f);
		prota->body->ApplyLinearImpulse(b2Vec2(1, 0), prota->body->GetWorldCenter(), true);
		moveState = MS_RIGHT;
		stopped = false;
	}
	if (key->isPushing(OF_KEY_LEFT))
	{
		float impulse = b2Max(prota->body->GetLinearVelocity().x - 0.01f, -0.5f);
		prota->body->ApplyLinearImpulse(b2Vec2(-1, 0), prota->body->GetWorldCenter(), true);
		moveState = MS_LEFT;
		stopped = false;
	}
	if (stopped)
	{
		prota->body->SetLinearVelocity(b2Vec2(prota->body->GetLinearVelocity().x*0.85f, prota->body->GetLinearVelocity().y));
	}

	if (key->isRelease('z'))
	{
		auto time = key->getTimePressed('z');
		std::cout << "Tiempo pulsado: " << time;
		float jumpImpulse = 80.0f;
		if (time > 100)
		{
			jumpImpulse = jumpImpulse + ofClamp(time*0.25f, 20, 100);
		}
		std::cout << " -> " << jumpImpulse << "\n";
		prota->body->ApplyLinearImpulse(b2Vec2(0, -jumpImpulse), prota->body->GetWorldCenter(), true);
		//prota->enableGravity(false);
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofDrawBitmapString("Cuerpos "+ ofToString(box2d.getBodyCount()), 10, 10);
	ofNoFill();
	npc->draw();
	platform->draw();
	for (size_t i = 0; i < circles.size(); i++) {
		//ofFill();
		circles[i].get()->draw();
	}

	for (size_t i = 0; i < rectangles.size(); i++) {
		rectangles[i].get()->draw();
	}
	ofFill();
	prota->draw();
	string info = "";
	info += "FPS: " + ofToString(ofGetFrameRate(), 1) + "\n";
	ofSetHexColor(0x444342);
	ofDrawBitmapString(info, 30, 30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

void ofApp::exit()
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 

}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
	if (e.a != NULL && e.b != NULL) {
		bodyUserData* udA = (bodyUserData*)e.a->GetBody()->GetUserData();
		bodyUserData* udB = (bodyUserData*)e.b->GetBody()->GetUserData();
		//One way wall
		if ((udA != nullptr && udB != nullptr) && (udA->entityType == 5 || udA->entityType == 5))
		{
			b2Body* thePlayer;
			if (udA->entityType == 1)
			{
				thePlayer = e.a->GetBody();
			}
			else
			{
				thePlayer = e.b->GetBody();
			}
			int numPoints = e.theContact->GetManifold()->pointCount;
			b2WorldManifold worldManifold;
			e.theContact->GetWorldManifold(&worldManifold);

			//check if contact points are moving downward
			for (int i = 0; i < numPoints; i++) {
				b2Vec2 pointVel = thePlayer->GetLinearVelocityFromWorldPoint(worldManifold.points[i]);
				if (pointVel.y > 0)
					return;//point is moving down, leave contact solid and exit
			}

			e.theContact->SetEnabled(false);
		}
		//end One way wall

		// if we collide with the ground we do not
		// want to play a sound. this is how you do that
		if (e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_polygon) 
		{
			
			bodyUserData* bProta;
			if (udA != nullptr && udA->entityType == 1)
			{
				bProta = udA;
			}
			else if (udB != nullptr && udB->entityType == 1)
			{
				bProta = udB;
			}

			if (prota != nullptr)
			{
				std::cout << "Personaje implicado en colisión\n";
			}
			//SoundData * aData = (SoundData*)e.a->GetBody()->GetUserData();
			//SoundData * bData = (SoundData*)e.b->GetBody()->GetUserData();
			//
			//if (aData) {
			//	aData->bHit = true;
			//	sound[aData->soundID].play();
			//}
			//
			//if (bData) {
			//	bData->bHit = true;
			//	sound[bData->soundID].play();
			//}
		}
	}
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e) {
	if (e.a != NULL && e.b != NULL) {
		e.theContact->SetEnabled(true);
		//SoundData * aData = (SoundData*)e.a->GetBody()->GetUserData();
		//SoundData * bData = (SoundData*)e.b->GetBody()->GetUserData();
		//
		//if (aData) {
		//	aData->bHit = false;
		//}
		//
		//if (bData) {
		//	bData->bHit = false;
		//}
	}
}
/*
std::vector<ofApp::protoBody> ofApp::getProtoBodiesFromXml(std::string xmlFilePath)
{
	std::vector<protoBody> bodiesInXml;
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
							std::cout << "\t\t\t Point at: (" << tempPoint.x << ", "<<tempPoint.y << ")\n";
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
*/