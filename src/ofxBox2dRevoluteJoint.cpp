/*
 *  ofxBox2dRevoluteJoint.cpp
 *  jointExample
 *
 *  Created by veketor on 12/04/20.
 *
 */

#include "ofxBox2dRevoluteJoint.h"
#include "ofxBox2d.h"

//----------------------------------------
ofxBox2dRevoluteJoint::ofxBox2dRevoluteJoint() {
	world = NULL;
	joint = NULL;
	alive = false;
}

//----------------------------------------
ofxBox2dRevoluteJoint::ofxBox2dRevoluteJoint(b2World* b2world, b2Body* body1, b2Body* body2, bool bCollideConnected) {
	ofxBox2dRevoluteJoint();
	setup(b2world, body1, body2, bCollideConnected);
}

//----------------------------------------
ofxBox2dRevoluteJoint::ofxBox2dRevoluteJoint(b2World* b2world, b2Body* body1, b2Body* body2, b2Vec2 anchor1, b2Vec2 anchor2, bool bCollideConnected) {
	ofxBox2dRevoluteJoint();
	setup(b2world, body1, body2, anchor1, anchor2, bCollideConnected);
}

//----------------------------------------
ofxBox2dRevoluteJoint::ofxBox2dRevoluteJoint(b2World* b2world, b2RevoluteJointDef jointDef) {
    ofxBox2dRevoluteJoint();
    setup(b2world, jointDef);
}

//----------------------------------------
void ofxBox2dRevoluteJoint::setup(b2World* b2world, b2Body* body1, b2Body* body2, bool bCollideConnected) {
	
	if(body1 == NULL || body2 == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dRevoluteJoint :: setup : - box2d body is NULL -");
		return;
	}
	
	b2Vec2 a1, a2;
	a1 = body1->GetWorldCenter();
	a2 = body2->GetWorldCenter();
	
	setup(b2world, body1, body2, a1, a2, bCollideConnected);
    
    alive = true;
}

//----------------------------------------
void ofxBox2dRevoluteJoint::setup(b2World* b2world, b2Body* body1, b2Body* body2, b2Vec2 anchor1, b2Vec2 anchor2, bool bCollideConnected) {

	if(body1 == NULL || body2 == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dRevoluteJoint :: setup : - box2d body is NULL -");
		return;
	}

	b2RevoluteJointDef jointDef;
	jointDef.bodyA = body1;
	jointDef.bodyB = body2;
	jointDef.localAnchorA.Set(anchor1.x, anchor1.y);
	jointDef.localAnchorB.Set(anchor2.x, anchor2.y);
	jointDef.collideConnected = bCollideConnected;

	//jointDef.Initialize(body1, body2, anchor1);// , anchor2);
	
    setup(b2world, jointDef);
}

//----------------------------------------
void ofxBox2dRevoluteJoint::setup(b2World* b2world, b2RevoluteJointDef jointDef) {

    setWorld(b2world);
    
    joint = /*(b2RevoluteJointDef*)*/world->CreateJoint(&jointDef);
	
	alive = true;
}

//----------------------------------------
//void ofxBox2dRevoluteJoint::setupMouseJoint(b2World* b2world, b2Body* bodyMouse, b2Body* bodyObj, float frequencyHz, float damping) {
//    jointType = e_mouseJoint;
//    b2MouseJointDef jointDef;
//    
//    jointDef.bodyA = bodyMouse;
//    jointDef.bodyB = bodyObj;
//    jointDef.target = bodyObj->GetWorldCenter();
//    jointDef.maxForce = bodyObj->GetMass()*1000;
//    jointDef.frequencyHz = frequencyHz;
//    jointDef.dampingRatio = damping;
//    
//    
//    setWorld(b2world);
//    
//    joint = world->CreateJoint(&jointDef);
//    
//    alive = true;
//}

//void ofxBox2dRevoluteJoint::updateTarget() {
//    if (joint && jointType == e_mouseJoint) {
//        ((b2MouseJoint *)joint)->SetTarget(joint->GetBodyA()->GetWorldCenter());
//    }
//}


//----------------------------------------
void ofxBox2dRevoluteJoint::setWorld(b2World* w) {
	if(w == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dRevoluteJoint :: setWorld : - box2d world needed -");	
		return;
	}
	world = w;
}

//----------------------------------------
bool ofxBox2dRevoluteJoint::isSetup() {
	if (world == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dRevoluteJoint :: world must be set!");
		return false;
	}
	if (joint == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dRevoluteJoint :: setup function must be called!");
		return false;
	}
	return true;
}


//----------------------------------------
void ofxBox2dRevoluteJoint::draw() {
	if(!alive) return;
	ofVec2f p1 = ofxBox2d::toOf(joint->GetAnchorA());
	ofVec2f p2 = ofxBox2d::toOf(joint->GetAnchorB());
	ofDrawLine(p1, p2);
}

//----------------------------------------
void ofxBox2dRevoluteJoint::destroy() {
	if (!isSetup()) return;
	if(joint) {
		world->DestroyJoint(joint);
	}
	joint = NULL;
	alive = false;
}



