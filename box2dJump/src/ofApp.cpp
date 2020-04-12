#include "ofApp.h"
#include <typeinfo>
#include <chrono>
#include <thread>
//#include <future>

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
	box2d.registerGrabbing(&cam);
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
	bodyUserData* blockUserData = new bodyUserData;
	blockUserData->entityType = 1; //1 is a block that permit go down (test)
	blockUserData->stunnedTimeout = 10.0f;
	rect->body->SetUserData(blockUserData);
	rectangles.push_back(rect);

	auto rectL = make_shared<ofxBox2dRect>();
	rectL->setPhysics(0.0, 0.5, 0.9);
	rectL->setup(box2d.getWorld(), 2, 2 + ofGetHeight()*0.5, 32, ofGetHeight());
	rectangles.push_back(rectL);

	auto rectR = make_shared<ofxBox2dRect>();
	rectR->setPhysics(0.0, 0.5, 0.9);
	rectR->setup(box2d.getWorld(), ofGetWindowWidth(), 2 + ofGetHeight()*0.5, 32, ofGetHeight());
	rectangles.push_back(rectR);

	auto rectT = make_shared<ofxBox2dRect>();
	rectT->setPhysics(0.0, 0.5, 0.9);
	rectT->setup(box2d.getWorld(), ofGetWindowWidth()*0.5, 0, ofGetWindowWidth(), 32);
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
	auto loaded = bodiesLoader.loadFromXml("test2.xml");
	auto pb = bodiesLoader.getProtoBody("coyote", "enemy");
	auto pb0 = bodiesLoader.getProtoBody("coyote");
	auto pb1 = bodiesLoader.getProtoBody("plataforma");
	auto pb2 = bodiesLoader.getProtoBody("logo");
	auto pb3 = bodiesLoader.getProtoBody("compuesto");
	auto pb4 = bodiesLoader.getProtoBody("lollipop");
	auto pb5 = bodiesLoader.getProtoBody("baboon");

	compoundBody = make_shared<ofxBox2dCompoundBody>();
	compoundBody->setup(box2d.getWorld(), bodiesLoader.getProtoBody("logo"));// pb5);
	auto pos = compoundBody->getPosition();
	auto isBody = compoundBody->isBody();
	compoundBody->setPosition(ofVec2f(690, 300));
	auto pos2 = compoundBody->getPosition();

	//PlayerRectCircleJoined
	 //player
	{
		/*
		auto rectL = make_shared<ofxBox2dRect>();
		rectL->setPhysics(0.0, 0.5, 0.9);
		rectL->setup(box2d.getWorld(), 2, 2 + ofGetHeight()*0.5, 32, ofGetHeight());
		*/

		/*
		std::shared_ptr <ofxBox2dRect> chavBox;
		shared_ptr<ofxBox2dCircle> chavCircle;
		shared_ptr<ofxBox2dJoint> chavJoint;
		*/

		//chavBox->setDensity(0.001f);
		chavBox = make_shared<ofxBox2dRect>();
		chavBox->setup(box2d.getWorld(), 200, 200, 25, 37.5, 0);
		chavBox->body->SetType(b2BodyType::b2_dynamicBody);
		chavBox->setFixedRotation(true);

		//chavCircle->setDensity(0.001f);
		chavCircle = make_shared<ofxBox2dCircle>();
		chavCircle->setup(box2d.getWorld(), 200, 225, 12.5f, false);
		chavCircle->body->SetType(b2BodyType::b2_dynamicBody);
		chavCircle->setFixedRotation(true);

		bodyUserData* chavUserData = new bodyUserData;
		chavUserData->entityType = 23; //23 mark the fixture as chav foot (23 for example...make an enum)
		chavUserData->stunnedTimeout = 10.0f;
		chavCircle->body->SetUserData(chavUserData);

		chavJoint = make_shared<ofxBox2dRevoluteJoint>(box2d.getWorld(), chavBox->body, chavCircle->body, b2Vec2(0, box2d.toB2d(18.75f)), b2Vec2(0, 0));
	}

	//camera
	cam.removeAllInteractions();
	//cam.addInteraction(ofEasyCam::TRANSFORM_ROTATE, OF_MOUSE_BUTTON_LEFT, OF_KEY_LEFT_CONTROL);
	cam.addInteraction(ofEasyCam::TRANSFORM_TRANSLATE_XY, OF_MOUSE_BUTTON_LEFT, OF_KEY_ALT);
	cam.addInteraction(ofEasyCam::TRANSFORM_TRANSLATE_Z, OF_MOUSE_BUTTON_RIGHT, OF_KEY_ALT);
	cam.enableOrtho();
	cam.setNearClip(-1000000);
	cam.setFarClip(1000000);
	cam.setVFlip(true);
	cam.setAutoDistance(false);
	cam.setPosition(glm::vec3(510, 250, 433));
	//auto scale = cam.getScale();
	int a= 0;
	a = a;

}

//--------------------------------------------------------------
void ofApp::update()
{
	if (key->isRelease(OF_KEY_F1))
	{
		if (box2d.getWorld()->GetGravity().y != 0)
		{
			box2d.setGravity(0, 0);
		}
		else
		{
			box2d.setGravity(0, 30);
		}
	}
	box2d.update();
	float lastFrameTime = ofGetLastFrameTime();
	//std::cout << lastFrameTime << "\n";
	if (lastFrameTime > 0.5)
	{
		lastFrameTime = 0.016f;
	}
	moveState = MS_STOP;
	bool stopped = true;

	if (key->isRelease(OF_KEY_DOWN))
	{
		bool _isOnPlatform = false;
		for (b2ContactEdge* edge = prota->body->GetContactList(); edge; edge = edge->next)
		{
			bodyUserData* otherUserData = (bodyUserData*)edge->other->GetUserData();
			if (otherUserData != nullptr)
			{
				if (otherUserData->entityType == 5 || otherUserData->entityType == 1)
				_isOnPlatform = true;
			}
		}
		if (_isOnPlatform)
		{
			auto fixtures = prota->body->GetFixtureList();
			if (fixtures != nullptr)
			{
				std::cout << "Desactivando colisiones\n";
				fixtures->SetSensor(true);
				while (fixtures->GetNext())
				{
					fixtures->SetSensor(true);
				}

			}
			//as a hack, after 0.25 seconds, change to non sensor.
			//std::async(std::launch::async, [=]()
			std::thread([this]
			{
				std::this_thread::sleep_for(std::chrono::milliseconds{ 250 });
				auto fixtures = prota->body->GetFixtureList();
				if (fixtures != nullptr)
				{
					std::cout << "Activando colisiones\n";
					fixtures->SetSensor(false);
					while (fixtures->GetNext())
					{
						fixtures->SetSensor(false);
					}
				}
			}).detach();
		}
	}

	if (key->isPushing(OF_KEY_RIGHT)) {
		float impulse = b2Min(b2Max(prota->body->GetLinearVelocity().x + 0.0001f, 0.8f), 1.0f);
		std::cout << impulse << "\n";
		prota->body->ApplyLinearImpulse(b2Vec2(impulse, 0), prota->body->GetWorldCenter(), true);
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

	if (key->isRelease('r'))
	{

		cam.setPosition(glm::vec3(510, 250, 433));
		cam.setScale(glm::vec3(1, 1, 1));
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
	//Max Speed control
	if (prota->body->GetLinearVelocity().x > 10)
	{
		prota->body->SetLinearVelocity(b2Vec2(10, prota->body->GetLinearVelocity().y));
	}
	else if (prota->body->GetLinearVelocity().x < -10)
	{
		prota->body->SetLinearVelocity(b2Vec2(-10, prota->body->GetLinearVelocity().y));
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofDrawBitmapString("Cuerpos "+ ofToString(box2d.getBodyCount()), 10, 10);
	string info = "";
	info += "FPS: " + ofToString(ofGetFrameRate(), 1) + "\n";
	ofSetHexColor(0x444342);
	ofDrawBitmapString(info, 30, 30);
	string camPos = ofToString(cam.getPosition().x) + ", " + ofToString(cam.getPosition().y) + ", " + ofToString(cam.getPosition().z);
	ofDrawBitmapStringHighlight(camPos, 10, 50);
	ofDrawBitmapStringHighlight("Use alt+MouseLeft/Right to move camera", 10, 70);
	ofDrawBitmapStringHighlight("Use arrows and z key to control the actor", 10, 90);
	auto scale = cam.getScale();
	ofDrawBitmapStringHighlight("Scale factor: ("+ofToString(scale.x)+", "+ ofToString(scale.y) + ", " + ofToString(scale.z)+")", 10, 110);
	ofDrawBitmapStringHighlight("PlayerSpeed: " + ofToString(prota->body->GetLinearVelocity().x), 10, 130);
	ofDrawBitmapString("Press F1 to enable/disable gravity force", 10, 150);
	ofDrawBitmapString("(" + ofToString(cam.screenToWorld({ mouseX, mouseY, 0 }).x) + ", " + ofToString(cam.screenToWorld({ mouseX, mouseY, 0 }).y) + ")", mouseX + 16, mouseY + 16);
	cam.begin();
		ofNoFill();
		npc->draw();
		platform->draw();
		for (size_t i = 0; i < circles.size(); i++) 
		{
			//ofFill();
			circles[i].get()->draw();
		}

		for (size_t i = 0; i < rectangles.size(); i++) 
		{
			rectangles[i].get()->draw();
		}

		ofFill();
		prota->draw();

		compoundBody->draw();
		
		chavBox->draw();
		chavCircle->draw();
		
	cam.end();
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