#pragma once

#include "ofMain.h"
#include <nuitrack/Nuitrack.h>
#include "ofxNuitrack.h"

using namespace tdv::nuitrack;

class Bone {
public:
	Bone(JointType _from, JointType _to, glm::vec3 _direction) {
		from = _from;
		to = _to;
		direction = _direction;
	}

	JointType from;
	JointType to;
	glm::vec3 direction;
};

class ofApp: public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void updatePointcloud();
		void drawSkeleton(Skeleton &s, ofColor color);
		void drawJoint(Joint &j, ofColor color);
		void drawBones(vector<Joint> joints, ofColor color);
		void drawUsers();
		void drawPointcloud();

		bool bNeedPointcloudUpdate;
		ofxnui::TrackerRef tracker;
		ofTexture rgbTex;
		ofTexture depthTex;		
		glm::vec2 rgbFrameSize;
		glm::vec2 depthFrameSize;

		vector<Skeleton> skeletons;
		vector<User> users;

		ofVboMesh pc;				// pointcloud
		ofEasyCam cam;

		float cameraHeight = 1.0;	// height of camera in realspace (m)
		glm::vec3 floorPoint;
		glm::vec3 floorNormal;
};
