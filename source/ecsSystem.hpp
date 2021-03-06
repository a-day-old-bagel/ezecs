/*
 * Copyright (c) 2016 Galen Cochrane
 * Galen Cochrane <galencochrane@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "ecsState.generated.hpp"

namespace ezecs {

  typedef rtu::Delegate<bool(const entityId& id)> entNotifyHandler;
  static bool passThrough(const entityId &) { return true; }

  struct IdRegistry {
    std::vector<entityId> ids;
    entNotifyHandler discoverHandler;
    entNotifyHandler forgetHandler;
    explicit IdRegistry(entNotifyHandler&& discoverHandler = RTU_FUNC_DLGT(passThrough),
                        entNotifyHandler&& forgetHandler   = RTU_FUNC_DLGT(passThrough))
                        : discoverHandler(discoverHandler), forgetHandler(forgetHandler) { }
  };
	static void discover(const entityId& id, void* data) {
		auto registry = reinterpret_cast<IdRegistry*>(data);
		if (registry->discoverHandler(id)) {
			registry->ids.push_back(id);
		}
	}
	static void forget(const entityId& id, void* data) {
		auto registry = reinterpret_cast<IdRegistry*>(data);
		auto position = std::find(registry->ids.begin(), registry->ids.end(), id);
		if (position != registry->ids.end()) {
			if (registry->forgetHandler(id)) {
				registry->ids.erase(position);
			}
		}
	}

  template<typename Derived_System>
  class System
  {
    private:
      bool paused = false;
      Derived_System& sys();

    protected:
      std::string name = "Generic System";
      State* state;
      std::vector<IdRegistry> registries;

    public:
      explicit System(State* state, std::vector<ezecs::compMask> &&requiredComps);
      virtual ~System();
      void tick(double dt);
      void pause();
      void resume();
      void clean();
      bool isPaused();
  };

  template<typename Derived_System>
  System<Derived_System>::System(State* state, std::vector<ezecs::compMask> &&requiredComps) : state(state) {
	  registries.resize(requiredComps.size());
	  for (size_t i = 0; i < requiredComps.size(); ++i) {
		  state->listenForLikeEntities(
					  requiredComps[i],
					  EntNotifyDelegate{ RTU_FUNC_DLGT(discover), requiredComps[i], &registries[i] },
					  EntNotifyDelegate{ RTU_FUNC_DLGT(forget), requiredComps[i], &registries[i] }
		  );
	  }
  }
  template<typename Derived_System>
  System<Derived_System>::~System() {
    // TODO: Remove callbacks from State
  }
  template<typename Derived_System>
  Derived_System& System<Derived_System>::sys() {
    return *static_cast<Derived_System*>(this);
  }
  template<typename Derived_System>
  void System<Derived_System>::tick(double dt) {
    sys().onTick(dt);
  }
  template<typename Derived_System>
  void System<Derived_System>::pause(){
    if (!paused){
      paused = true;
    }
  }
  template<typename Derived_System>
  void System<Derived_System>::resume(){
    if (paused){
      paused = false;
    }
  }
  template<typename Derived_System>
  void System<Derived_System>::clean(){
    sys().onClean();
    for (auto registry : registries) {
      registry.ids.clear();
    }
  }
  template<typename Derived_System>
  bool System<Derived_System>::isPaused(){
    return paused;
  }
}
