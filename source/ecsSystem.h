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
#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

#include <string>
#include <vector>
#include <algorithm>
#include "ecsState.h"

namespace ecs {

  typedef Delegate<bool(const entityId& id)> entNotifyHandler;
  static bool doNothing(const entityId&) { return true; }

  struct IdRegistry {
    std::vector<entityId> ids;
    entNotifyHandler discoverHandler;
    entNotifyHandler forgetHandler;
    IdRegistry(entNotifyHandler&& discoverHandler = DELEGATE_NOCLASS(doNothing),
               entNotifyHandler&& forgetHandler   = DELEGATE_NOCLASS(doNothing))
               : discoverHandler(discoverHandler), forgetHandler(forgetHandler) { }
  };

  template<typename Derived_System>
  class System
  {
    private:
      bool paused = false;
      Derived_System& sys();

    protected:
      State* state;
      std::vector<IdRegistry> registries;

    public:
      System(State* state);
      bool init();
      void tick(float dt);
      void pause();
      void resume();
      void clean();
      bool isPaused();
  };

  template<typename Derived_System>
  System<Derived_System>::System(State* state)
      : state(state) { }
  template<typename Derived_System>
  Derived_System& System<Derived_System>::sys() {
    return *static_cast<Derived_System*>(this);
  }
  static void discover(const entityId& id, void* data) {
    IdRegistry* registry = reinterpret_cast<IdRegistry*>(data);
    if (registry->discoverHandler(id)) {
      registry->ids.push_back(id);
    }
  }
  static void forget(const entityId& id, void* data) {
    IdRegistry* registry = reinterpret_cast<IdRegistry*>(data);
    std::vector<entityId>::iterator position = std::find(registry->ids.begin(), registry->ids.end(), id);
    if (position != registry->ids.end()) {
      if (registry->forgetHandler(id)) {
        registry->ids.erase(position);
      }
    }
  }
  template<typename Derived_System>
  bool System<Derived_System>::init() {
    registries.resize(sys().requiredComponents.size());
    for (int i = 0; i < sys().requiredComponents.size(); i++) {
      state->listenForLikeEntities(
           sys().requiredComponents[i],
           EntNotifyDelegate{ DELEGATE_NOCLASS(discover), sys().requiredComponents[i], &registries[i] },
           EntNotifyDelegate{ DELEGATE_NOCLASS(forget), sys().requiredComponents[i], &registries[i] }
      );
    }
    return sys().onInit();
  }
  template<typename Derived_System>
  void System<Derived_System>::tick(float dt) {
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

#endif //ECS_SYSTEM_H
