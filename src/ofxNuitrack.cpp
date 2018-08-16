#include "ofxNuitrack.h"

using namespace tdv;

namespace ofxnui {

	Tracker::Tracker() {
		onRGBHandle = 0;
		onDepthHandle = 0;
		onUserHandle = 0;
		onHandHandle = 0;
		onSkeletonHandle = 0;
	}

	Tracker::~Tracker() {
		nuitrack::Nuitrack::release();
	}

	TrackerRef Tracker::create()
	{
		return TrackerRef(new Tracker());
	}

	void Tracker::init(string configPath) {
		try {
			nuitrack::Nuitrack::init(configPath);
		}
		catch (const nuitrack::Exception &e) {
			ofLogError() << "Error initializing Nuitrack: " << e.what() << "\nPlease make sure a device is connected";
			return;
		}

		ofLogNotice() << "Initialized Nuitrack succesfully";
	}

	void Tracker::setConfigValue(const std::string& key, const std::string& value) {
		nuitrack::Nuitrack::setConfigValue(key, value);
	}

	string Tracker::getConfigValue(const std::string& key) {
		return nuitrack::Nuitrack::getConfigValue(key);
	}

	void Tracker::run() {
		try {
			nuitrack::Nuitrack::run();
		}
		catch (const nuitrack::Exception &e) {
			ofLogError() << "Error running Nuitrack: " << e.what();
		}
	}

	void Tracker::poll() {
		try
		{
			if (rgbTracker) nuitrack::Nuitrack::update(rgbTracker);
			if (depthTracker) nuitrack::Nuitrack::update(depthTracker);
			if (userTracker) nuitrack::Nuitrack::update(userTracker);
			if (handTracker) nuitrack::Nuitrack::update(handTracker);
			if (skeletonTracker) nuitrack::Nuitrack::update(skeletonTracker);
		}
		catch (const nuitrack::Exception& e)
		{
			ofLogError() << "Error updating Nuitrack: " << e.what();
		}
	}

	nuitrack::User Tracker::getUser(int id)
	{
		nuitrack::User invalidUser;
		invalidUser.id = id - 1000;

		if (!userTracker) {
			return invalidUser;
		}

		auto userFrame = userTracker->getUserFrame();
		if (!userFrame) {
			return invalidUser;
		}

		auto users = userFrame->getUsers();
		if (users.empty()) {
			return invalidUser;
		}

		for (auto u : users) {
			if (u.id == id) {
				return u;
			}
		}

		return invalidUser;
	}

	void Tracker::setIssuesCallback(function<void(nuitrack::IssuesData::Ptr)> onIssues)
	{
		if (onIssuesHandle) {
			unbindIssuesCallback();
		}

		onIssuesHandle = nuitrack::Nuitrack::connectOnIssuesUpdate(onIssues);
	}

	void Tracker::unbindIssuesCallback()
	{
		if (!onIssuesHandle) {
			return;
		}

		nuitrack::Nuitrack::disconnectOnIssuesUpdate(onIssuesHandle);
		onIssuesHandle = 0;
	}


	void Tracker::setRGBCallback(function<void(nuitrack::RGBFrame::Ptr)> onRGB)
	{
		if (onRGBHandle) {
			unbindRGBCallback();
		}

		if (!rgbTracker) {
			rgbTracker = nuitrack::ColorSensor::create();
		}

		onRGBHandle = rgbTracker->connectOnNewFrame(onRGB);
	}

	void Tracker::unbindRGBCallback()
	{
		if (rgbTracker) {
			rgbTracker->disconnectOnNewFrame(onRGBHandle);
		}

		onRGBHandle = 0;
	}

	void Tracker::setDepthCallback(function<void(nuitrack::DepthFrame::Ptr)> onDepth)
	{
		if (onDepthHandle) {
			unbindDepthCallback();
		}

		if (!depthTracker) {
			depthTracker = nuitrack::DepthSensor::create();
		}

		onDepthHandle = depthTracker->connectOnNewFrame(onDepth);
	}

	void Tracker::unbindDepthCallback()
	{
		if (depthTracker) {
			depthTracker->disconnectOnNewFrame(onDepthHandle);
		}

		onDepthHandle = 0;
	}

	void Tracker::setUserCallback(function<void(nuitrack::UserFrame::Ptr)> onUser)
	{
		if (onUserHandle) {
			unbindUserCallback();
		}

		if (!userTracker) {
			userTracker = nuitrack::UserTracker::create();
		}

		onUserHandle = userTracker->connectOnUpdate(onUser);
	}

	void Tracker::unbindUserCallback()
	{
		if (userTracker) {
			userTracker->disconnectOnUpdate(onUserHandle);
		}

		onUserHandle = 0;
	}

	void Tracker::setHandCallback(function<void(nuitrack::HandTrackerData::Ptr)> onHand)
	{
		if (onHandHandle) {
			unbindHandCallback();
		}

		if (!handTracker) {
			handTracker = nuitrack::HandTracker::create();
		}

		onHandHandle = handTracker->connectOnUpdate(onHand);
	}

	void Tracker::unbindHandCallback()
	{
		if (handTracker) {
			handTracker->disconnectOnUpdate(onHandHandle);
		}

		onHandHandle = 0;
	}

	void Tracker::setSkeletonCallback(function<void(nuitrack::SkeletonData::Ptr)> onSkeleton)
	{
		if (onSkeletonHandle) {
			unbindSkeletonCallback();
		}

		if (!skeletonTracker) {
			skeletonTracker = nuitrack::SkeletonTracker::create();
		}

		onSkeletonHandle = skeletonTracker->connectOnUpdate(onSkeleton);
	}

	void Tracker::unbindSkeletonCallback()
	{
		if (skeletonTracker) {
			skeletonTracker->disconnectOnUpdate(onSkeletonHandle);
		}

		onSkeletonHandle = 0;
	}
}