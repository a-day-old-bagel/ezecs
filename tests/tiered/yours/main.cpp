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

#include "coreSystems.hpp"

#define GET_TIME std::chrono::high_resolution_clock::now();
#define DURATION std::chrono::duration<double, std::milli>
#define CHECK(compOpReturn) EZECS_CHECK_PRINT(EZECS_ERR(compOpReturn))

using namespace ezecs;

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