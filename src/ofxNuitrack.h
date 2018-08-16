#pragma once

#include "ofMain.h"
#include <nuitrack/Nuitrack.h>

namespace ofxnui {

	using namespace tdv;
	using namespace std;

	typedef std::shared_ptr<class Tracker> TrackerRef;

	class Tracker {
	public:
		Tracker();
		~Tracker();
		static TrackerRef create();

		// empty configPath means default config
		void init(string configPath = "");

		void setConfigValue(const std::string& key, const std::string& value);
		string getConfigValue(const std::string& key);

		// start the trackers
		void run();

		// poll for new data
		void poll();

		nuitrack::User getUser(int id);

		// callbacks
		// setting a callback binds it immediately
		void setIssuesCallback(function<void(nuitrack::IssuesData::Ptr)> onIssues);
		void unbindIssuesCallback();

		void setRGBCallback(function<void(nuitrack::RGBFrame::Ptr)> onRGB);
		void unbindRGBCallback();

		void setDepthCallback(function<void(nuitrack::DepthFrame::Ptr)> onDepth);
		void unbindDepthCallback();

		void setUserCallback(function<void(nuitrack::UserFrame::Ptr)> onUser);
		void unbindUserCallback();

		void setHandCallback(function<void(nuitrack::HandTrackerData::Ptr)> onHand);
		void unbindHandCallback();

		void setSkeletonCallback(function<void(nuitrack::SkeletonData::Ptr)> onSkeleton);
		void unbindSkeletonCallback();

		// Vector3 -> vec3
		static glm::vec3 fromVector3(nuitrack::Vector3 v) {
			return glm::vec3(v.x, v.y, v.z);
		}

		// Orientation -> glm::quat
		static glm::quat fromOrientation(nuitrack::Orientation o) {
			const float *rotMatrix = o.matrix;
			float transposed[9] = {
				rotMatrix[0], rotMatrix[3], rotMatrix[6],
				rotMatrix[1], rotMatrix[4], rotMatrix[7],
				rotMatrix[2], rotMatrix[5], rotMatrix[8]
			};
			auto rotation = glm::make_mat3x3<float>(transposed);
			return glm::toQuat(rotation);
		}

	protected:
		uint64_t onIssuesHandle;
		uint64_t onRGBHandle;
		uint64_t onDepthHandle;
		uint64_t onUserHandle;
		uint64_t onHandHandle;
		uint64_t onSkeletonHandle;

	public:
		nuitrack::ColorSensor::Ptr rgbTracker;
		nuitrack::DepthSensor::Ptr depthTracker;
		nuitrack::UserTracker::Ptr userTracker;
		nuitrack::HandTracker::Ptr handTracker;
		nuitrack::SkeletonTracker::Ptr skeletonTracker;
	};
}