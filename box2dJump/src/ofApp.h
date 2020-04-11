#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "b2Body.h"
#include "ofxKeyboard.h"

class ofApp : public ofBaseApp{

	public:
		enum _moveState {
			MS_STOP,
			MS_LEFT,
			MS_RIGHT,
		};

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void exit();
		
		ofxBox2d	box2d;
		shared_ptr<ofxBox2dRect> prota;
		vector		<shared_ptr<ofxBox2dCircle> >	circles;
		vector		<shared_ptr<ofxBox2dRect>>		rectangles;

		void contactStart(ofxBox2dContactArgs &e);
		void contactEnd(ofxBox2dContactArgs &e);

		struct bodyUserData 
		{
			int entityType;
			float health;
			float stunnedTimeout;
		};
		std::shared_ptr<ofxBox2dRect>  platform;
		std::shared_ptr <ofxBox2dRect> npc;
		b2Body* createdBody;
		oxfProtoBodies bodiesLoader;
		std::shared_ptr <ofxBox2dCompoundBody> compoundBody;
	private:
		ofxKeyboard* key;
		_moveState moveState;
		ofEasyCam cam;
};