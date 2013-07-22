#pragma once

#include "SearchExperiment.h"

namespace SparCraft {
class BuildingPlacementExperiment:public SearchExperiment {
		public:
	BuildingPlacementExperiment(const std::string & configFile);
	virtual ~BuildingPlacementExperiment();
};

}
