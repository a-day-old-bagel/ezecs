 
#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#include "ezecs.hpp"

struct TestSystem : public ezecs::System<TestSystem> {
	std::stringstream outputLog;
	std::vector<ezecs::compMask> requiredComponents = {
				ezecs::FOOCOMP | ezecs::BAR_COMP,
				ezecs::MEHCOMP
	};
	explicit TestSystem(ezecs::State *state);
	bool onInit();
	void onTick(double dt);
	void deInit();
	bool onDiscoverFooBar(const ezecs::entityId &id);
	bool onForgetFooBar(const ezecs::entityId &id);
	bool onDiscoverMeh(const ezecs::entityId &id);
	bool onForgetMeh(const ezecs::entityId &id);
};