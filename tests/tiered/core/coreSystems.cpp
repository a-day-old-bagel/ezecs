
#include "coreSystems.hpp"

using namespace ezecs;

TestSystem::TestSystem(State *state) : System(state) {

}

bool TestSystem::onInit() {
	registries[0].discoverHandler = RTU_MTHD_DLGT(&TestSystem::onDiscoverFooBar, this);
	registries[0].forgetHandler = RTU_MTHD_DLGT(&TestSystem::onForgetFooBar, this);
	registries[1].discoverHandler = RTU_MTHD_DLGT(&TestSystem::onDiscoverMeh, this);
	registries[1].forgetHandler = RTU_MTHD_DLGT(&TestSystem::onForgetMeh, this);
	outputLog << "TEST SYSTEM INITIALIZED." << std::endl;
	return true;
}

void TestSystem::onTick(double dt) {
	outputLog << "TEST SYSTEM TICK TIME (ms): " << dt << "; bars say: ";
	for (auto id : registries[0].ids) {
		Bar_Comp* bar;
		state->getBar_Comp(id, &bar);
		bar->number += 0.2f;
		outputLog << bar->number << ", ";
	}
	outputLog << std::endl;
}

void TestSystem::deInit() {
	outputLog << "TEST SYSTEM DESTROYED." << std::endl;
	std::cout << outputLog.str();
}

bool TestSystem::onDiscoverFooBar(const entityId &id) {
	outputLog << "TEST SYSTEM DISCOVERED A FOOBAR: " << id << std::endl;
	return true;
}

bool TestSystem::onForgetFooBar(const entityId &id) {
	outputLog << "TEST SYSTEM FORGOT A FOOBAR: " << id << std::endl;
	return true;
}

bool TestSystem::onDiscoverMeh(const entityId &id) {
	outputLog << "TEST SYSTEM DISCOVERED A MEH: " << id << std::endl;
	return true;
}

bool TestSystem::onForgetMeh(const entityId &id) {
	outputLog << "TEST SYSTEM FORGOT A MEH: " << id << std::endl;
	return true;
}