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
#ifndef EZECS_ECSCONFIG_HPP
#define EZECS_ECSCONFIG_HPP

#include "ezecs.hpp"

using namespace ezecs;

namespace {

  // BEGIN DECLARATIONS

  struct FooComp : public Component<FooComp> {
    int fakeA, fakeB;

    FooComp(int fakeA, int fakeB);
  };
  EZECS_COMPONENT_DEPENDENCIES(FooComp)

  class BarComp : public Component<BarComp> {
      float number;

      BarComp(float number);
  };
  EZECS_COMPONENT_DEPENDENCIES(BarComp, FooComp)

  struct MehComp : public Component<MehComp> {
    char boo, hoo;

    MehComp(char boo, char hoo);
  };
  EZECS_COMPONENT_DEPENDENCIES(MehComp, FooComp, BarComp)

  // END DECLARATIONS

  // BEGIN DEFINITIONS

  FooComp::FooComp(int fakeA, int fakeB)
      : fakeA(fakeA), fakeB(fakeB) {}

  BarComp::BarComp(float number)
      : number(number) {}

  MehComp::MehComp(char boo, char hoo)
      : boo(boo), hoo(hoo) {}

  // END DEFINITIONS

}

#endif //EZECS_ECSCONFIG_HPP
