#include "ofApp.h"

using namespace tdv::nuitrack;
using namespace glm;
//--------------------------------------------------------------
void ofApp::setup(){

	ofSetWindowPosition(0, 0);
	ofSetWindowShape(1920, 1080);
	ofSetFrameRate(60);
	ofEnableAlphaBlending();

	pc.setMode(OF_PRIMITIVE_POINTS);

	tracker = ofxnui::Tracker::create();
	tracker->init();

	bool bUseRealsense = true;
	bool bPlayFile = true;

	if (bUseRealsense) {
		tracker->setConfigValue("Realsense2Module.Depth.FPS", "30");
		tracker->setConfigValue("Realsense2Module.Depth.RawWidth", "1280");
		tracker->setConfigValue("Realsense2Module.Depth.RawHeight", "720");
		tracker->setConfigValue("Realsense2Module.Depth.ProcessWidth", "1280");
		tracker->setConfigValue("Realsense2Module.Depth.ProcessHeight", "720");
		tracker->setConfigValue("Realsense2Module.Depth.ProcessMaxDepth", "7000");

		tracker->setConfigValue("Realsense2Module.RGB.FPS", "30");
		tracker->setConfigValue("Realsense2Module.RGB.RawWidth", "1280");
		tracker->setConfigValue("Realsense2Module.RGB.RawHeight", "720");
		tracker->setConfigValue("Realsense2Module.RGB.ProcessWidth", "1280");
		tracker->setConfigValue("Realsense2Module.RGB.ProcessHeight", "720");

		// post processing setting
		tracker->setConfigValue("Realsense2Module.Depth.PostProcessing.SpatialFilter.spatial_alpha", "0.1");
		tracker->setConfigValue("Realsense2Module.Depth.PostProcessing.SpatialFilter.spatial_delta", "50");

		if (bPlayFile) {
			string path = ofToDataPath("test.bag", true); // must be absolute path
			tracker->setConfigValue("Realsense2Module.FileRecord", path);
		}
	}
	else
	{
		// Setting fpr Astra etc
		if (bPlayFile) {
			string path = ofToDataPath("test.oni", true); //must be absolute path
			tracker->setConfigValue("OpenNIModule.FileRecord", path);
		}
	}

	tracker->setConfigValue("Segmantation.MAX_DISTANCE", "7000");
	tracker->setConfigValue("Skeletonization.MaxDistance", "7000");
	//tracker->setConfigValue("GestureRecognition.WavingEnabled", "true");  does not work?

	tracker->setIssuesCallback([this](IssuesData::Ptr data) {
		auto issue = data->getIssue<Issue>();
		if (issue) {
			ofLogNotice() << "Issue detected! " << issue->getName() << " [" << issue->getId() << "]";
		}
	});

	tracker->setRGBCallback([this](RGBFrame::Ptr data) {	
		try {
			rgbFrameSize.x = data->getCols();
			rgbFrameSize.y = data->getRows();
			const Color3 *colorData = data->getData();
			uint8_t *colorDataPtr = (uint8_t *)colorData;
			ofPixels pix;

			string fileName = tracker->getConfigValue("Realsense2Module.FileRecord");
			if (fileName == "") {
				//Live feed = BGR
				pix.setFromPixels(colorDataPtr, rgbFrameSize.x, rgbFrameSize.y, OF_PIXELS_BGR);
			}
			else 
			{
				// BAG file = RGB
				pix.setFromPixels(colorDataPtr, rgbFrameSize.x, rgbFrameSize.y, OF_PIXELS_RGB);
			}
			rgbTex.loadData(pix);

		}
		catch (const Exception &e) {
			ofLogWarning() << "Expeption : " << e.what();
		}
	});

	tracker->setDepthCallback([this](DepthFrame::Ptr data) {
		try {

			depthFrameSize.x = data->getCols();
			depthFrameSize.y = data->getRows();
			const unsigned short *depthData = data->getData();
			ofShortPixels pix;
			pix.setFromPixels(depthData, depthFrameSize.x, depthFrameSize.y, OF_PIXELS_GRAY);
			depthTex.loadData(pix);
			bNeedPointcloudUpdate = true;
		}
		catch (const Exception &e) {
			ofLogWarning() << "Expeption : " << e.what();
		}
	});


	tracker->setSkeletonCallback([this](SkeletonData::Ptr data) {
		try{
			skeletons = data->getSkeletons();
		}
		catch (const Exception &e) {
			ofLogWarning() << "Expeption : " << e.what();
		}
	});

	tracker->setUserCallback([this](UserFrame::Ptr data) {
		try{
			floorPoint = ofxnui::Tracker::fromVector3(data->getFloor()) / vec3(1000);
			floorNormal = ofxnui::Tracker::fromVector3(data->getFloorNormal());
			users = data->getUsers();
		}	
		catch (const Exception &e) {
			ofLogWarning() << "Expeption : " << e.what();
		}
	});

	tracker->run();
}

//--------------------------------------------------------------
void ofApp::update(){
	try {
		tracker->poll();
	}
	catch (const Exception &e) {
		ofLogWarning() << "Expeption : " << e.what();
	}

	updatePointcloud();
}

void ofApp::updatePointcloud() {
	if (bNeedPointcloudUpdate) {

		pc.clear();

		DepthSensor::Ptr dtracker = tracker->depthTracker;
		DepthFrame::Ptr dframe = dtracker->getDepthFrame();
		int row = dframe->getRows();
		int col = dframe->getCols();
		int skip = 4;
		const unsigned short * data = dframe->getData();

		for (int y=0; y<row; y+=skip) {
			for (int x=0; x<col; x+=skip) {
				int index = y*col + x;
				unsigned short d = data[index];
				if (d > 0) {
					Vector3 v = dtracker->convertProjToRealCoords(x, y, d);
					pc.addVertex(glm::vec3(v.x, v.y, v.z)*0.001);
					ofFloatColor c;
					float g = ofMap(d, 0, 5000, 0.1, 1.0, true);
					c.set(0, g, 0, 0.9);
					pc.addColor(c);
					
				}
			}
		}

		bNeedPointcloudUpdate = false;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(60);

	cam.begin();
	ofScale(300, 300, -300);
	ofDrawAxis(0.1);
	
	// fixed camera height floor
	ofPushMatrix();
	ofTranslate(0, -cameraHeight, 0);
	ofRotateZDeg(90);
	ofSetColor(0, 200);
	ofDrawGridPlane(1, 5, true);
	ofPopMatrix();
	
	// floor detection (but ignore x, z value)
	ofPushMatrix();
	ofTranslate(0, floorPoint.y, 0);
	ofRotateZDeg(90);
	ofSetColor(200, 50);
	ofDrawGridPlane(1, 5, false);
	ofPopMatrix();
	
	drawPointcloud();

	for (int i = 0; i < skeletons.size(); i++) {
		auto s = skeletons[i];
		drawSkeleton(s, ofColor(0,0,255));
	}

	drawUsers();
	cam.end();

	if (rgbTex.isAllocated()) {
		ofSetColor(255);
		rgbTex.draw(0, 0, rgbFrameSize.x/3, rgbFrameSize.y/3);
	}

	if (depthTex.isAllocated()) {
		ofSetColor(255);
		depthTex.draw(rgbFrameSize.x/3, 0, depthFrameSize.x/3, depthFrameSize.y/3);
	}

	ofDrawBitmapStringHighlight("fps       " + ofToString(ofGetFrameRate(), 0), 0, 10);
	ofDrawBitmapStringHighlight("skeletons " + ofToString(skeletons.size(), 0), 0, 30);

}

void ofApp::drawSkeleton(Skeleton &s, ofColor color) {
	auto joints = s.joints;
	for (auto j : joints) {
		drawJoint(j, color);
	}

	drawBones(joints, ofColor(100, 0, 250));
}

void ofApp::drawJoint(Joint &j, ofColor color) {
	if (j.type == JOINT_NONE) {
		return;
	}

	if (j.confidence < 0.15) {
		return;
	}

	// real pos is in mm, lets convert to m
	glm::vec3 pos = ofxnui::Tracker::fromVector3(j.real) / glm::vec3(1000);

	ofPushMatrix();
	ofTranslate(pos);
	ofSetColor(color);
	ofDrawBox(0,0,0, 0.025);
	ofPopMatrix();
}

void ofApp::drawBones(vector<Joint> joints, ofColor color) {
	//if (joints.size() < 3) {
	//	return;
	//}

	// rotation based on T pose
	// sdk reports rotations relative to T pose..
	static vector<Bone> bones = {
		Bone(JOINT_WAIST, JOINT_TORSO, vec3(0, 1, 0)),
		Bone(JOINT_TORSO, JOINT_NECK, vec3(0, 1, 0)),
		Bone(JOINT_NECK, JOINT_HEAD, vec3(0, 1, 0)),

		Bone(JOINT_LEFT_COLLAR, JOINT_LEFT_SHOULDER, vec3(1, 0, 0)),
		Bone(JOINT_LEFT_SHOULDER, JOINT_LEFT_ELBOW, vec3(1, 0, 0)),
		Bone(JOINT_LEFT_ELBOW, JOINT_LEFT_WRIST, vec3(1, 0, 0)),
		Bone(JOINT_LEFT_WRIST, JOINT_LEFT_HAND, vec3(1, 0, 0)),

		Bone(JOINT_WAIST, JOINT_LEFT_HIP, vec3(1, 0, 0)),
		Bone(JOINT_LEFT_HIP, JOINT_LEFT_KNEE, vec3(0, -1, 0)),
		Bone(JOINT_LEFT_KNEE, JOINT_LEFT_ANKLE, vec3(0, -1, 0)),

		Bone(JOINT_RIGHT_COLLAR, JOINT_RIGHT_SHOULDER, vec3(-1, 0, 0)),
		Bone(JOINT_RIGHT_SHOULDER, JOINT_RIGHT_ELBOW, vec3(-1, 0, 0)),
		Bone(JOINT_RIGHT_ELBOW, JOINT_RIGHT_WRIST, vec3(-1, 0, 0)),
		Bone(JOINT_RIGHT_WRIST, JOINT_RIGHT_HAND, vec3(-1, 0, 0)),

		Bone(JOINT_WAIST, JOINT_RIGHT_HIP, vec3(-1, 0, 0)),
		Bone(JOINT_RIGHT_HIP, JOINT_RIGHT_KNEE, vec3(0, -1, 0)),
		Bone(JOINT_RIGHT_KNEE, JOINT_RIGHT_ANKLE, vec3(0, -1, 0)),
	};

	for (int i = 0; i < bones.size(); i++) {
		float size = 0.05;
		auto j1 = joints[bones[i].from];
		auto j2 = joints[bones[i].to];

		if (j1.confidence < 0.15 || j2.confidence < 0.15) {
			continue;
		}

		glm::vec3 p1 = ofxnui::Tracker::fromVector3(j1.real) / glm::vec3(1000);
		glm::vec3 p2 = ofxnui::Tracker::fromVector3(j2.real) / glm::vec3(1000);
		ofSetColor(color);
		ofDrawLine(p1, p2);
	}
}

void ofApp::drawUsers() {

	for (auto & u : users) {

		int uid = u.id;
		const Vector3 & v = u.real;
		
		// draw user's center of mass
		ofPushMatrix();
		ofTranslate(v.x*0.001, v.y*0.001, v.z*0.001);
		ofFill();
		ofSetColor(255, 0, 0);
		ofDrawSphere(0,0,0,0.025);
		ofDrawBitmapString("USER:"+ofToString(uid), 0.09, -0.025);
		ofPopMatrix();
	}
}

void ofApp::drawPointcloud() {
	ofPushMatrix();
	glPointSize(1);
	pc.draw();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	string keyAlpha = "Realsense2Module.Depth.PostProcessing.SpatialFilter.spatial_alpha";

	switch(key) {
	case '1':
	{
		// folowing code is just for test that
		// We can not change config setting in run time
		// we can change config value but no effect.
		try {
			string maxDepth = tracker->getConfigValue(keyAlpha);
			float maxDepthInt = ofToInt(maxDepth);
			maxDepthInt += 0.1;
			string newVal = ofToString(maxDepthInt, 1);
			tracker->setConfigValue(keyAlpha, newVal);
		}catch(const Exception &e) {
			ofLogWarning() << "Can not set config value : " << e.what();
		}
		break;
	}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
