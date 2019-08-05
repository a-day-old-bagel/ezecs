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

#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>
#include "ezecs.hpp"

#define GET_TIME std::chrono::high_resolution_clock::now();
#define DURATION std::chrono::duration<double, std::milli>
#define CHECK(compOpReturn) EZECS_CHECK_PRINT(EZECS_ERR(compOpReturn))

using namespace ezecs;

struct TestSystem : public System<TestSystem> {
  std::stringstream outputLog;
  std::vector<compMask> requiredComponents = {
      FOOCOMP | BAR_COMP,
      MEHCOMP
  };
  TestSystem(State *state) : System(state) {

  }
  bool onInit() {
    registries[0].discoverHandler = RTU_MTHD_DLGT(&TestSystem::onDiscoverFooBar, this);
    registries[0].forgetHandler = RTU_MTHD_DLGT(&TestSystem::onForgetFooBar, this);
    registries[1].discoverHandler = RTU_MTHD_DLGT(&TestSystem::onDiscoverMeh, this);
    registries[1].forgetHandler = RTU_MTHD_DLGT(&TestSystem::onForgetMeh, this);
    outputLog << "TEST SYSTEM INITIALIZED." << std::endl;
    return true;
  }
  void onTick(double dt) {
    outputLog << "TEST SYSTEM TICK TIME (ms): " << dt << "; bars say: ";
    for (auto id : registries[0].ids) {
      Bar_Comp* bar;
      state->getBar_Comp(id, &bar);
      bar->number += 0.2f;
      outputLog << bar->number << ", ";
    }
    outputLog << std::endl;
  }
  void deInit() {
    outputLog << "TEST SYSTEM DESTROYED." << std::endl;
    std::cout << outputLog.str();
  }
  bool onDiscoverFooBar(const entityId &id) {
    outputLog << "TEST SYSTEM DISCOVERED A FOOBAR: " << id << std::endl;
    return true;
  }
  bool onForgetFooBar(const entityId &id) {
    outputLog << "TEST SYSTEM FORGOT A FOOBAR: " << id << std::endl;
    return true;
  }
  bool onDiscoverMeh(const entityId &id) {
    outputLog << "TEST SYSTEM DISCOVERED A MEH: " << id << std::endl;
    return true;
  }
  bool onForgetMeh(const entityId &id) {
    outputLog << "TEST SYSTEM FORGOT A MEH: " << id << std::endl;
    return true;
  }
};

int main(int argc, char *argv[]) {
  State state;
  TestSystem testSystem(&state);
  testSystem.init();
  auto thenTime = GET_TIME;

  for (int i = 0; i < 5; ++i) {
    auto nowTime = GET_TIME;
    DURATION dt = nowTime - thenTime;
    thenTime = nowTime;
    testSystem.tick(dt.count());
  }

  entityId ent0;
  CHECK( state.createEntity(&ent0)      );
  CHECK( state.addFooComp(ent0, 1, 2)  );
  CHECK( state.addBar_Comp(ent0, 1.5f) );

  for (int i = 0; i < 5; ++i) {
    auto nowTime = GET_TIME;
    DURATION dt = nowTime - thenTime;
    thenTime = nowTime;
    testSystem.tick(dt.count());
  }

  CHECK( state.addMehComp(ent0, 0, 0)  );

  for (int i = 0; i < 5; ++i) {
    auto nowTime = GET_TIME;
    DURATION dt = nowTime - thenTime;
    thenTime = nowTime;
    testSystem.tick(dt.count());
  }

  state.deleteEntity(ent0);
  testSystem.deInit();

  return 0;
}