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

// BEGIN INCLUDES

#include <ctime>

// END INCLUDES

using namespace ezecs;

namespace {

  // BEGIN DECLARATIONS

  struct HerpComp : public Component<HerpComp> {
    char sticks, stones;
    HerpComp(char sticks, char stones);
  };
  EZECS_COMPONENT_DEPENDENCIES(HerpComp, FooComp, Bar_Comp, DerpComp)

  // END DECLARATIONS

  // BEGIN DEFINITIONS

  HerpComp::HerpComp(char sticks, char stones)
      : sticks(sticks), stones(stones) {}

  // END DEFINITIONS

}

#endif //EZECS_ECSCONFIG_HPP
