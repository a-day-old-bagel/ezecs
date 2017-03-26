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
#ifndef ECS_COMPONENTS_H
#define ECS_COMPONENTS_H

#include <stdint.h>
#include "ecsDelegate.hpp"

namespace ezecs {

  typedef uint32_t compMask;
  typedef uint32_t entityId;

  template <typename Derived>
  struct Component {
    static compMask requiredComps;
    static compMask dependentComps;
    static compMask flag;
  };
  
  /*
   * The existence component is necessary for all entities.
   */
  struct Existence : public Component<Existence> {
    compMask componentsPresent = 0;
    bool flagIsOn(int compType);
    bool passesPrerequisitesForAddition(compMask mask);
    bool passesDependenciesForRemoval(compMask mask);
    void turnOnFlags(compMask mask);
    void turnOffFlags(compMask mask);
  };

  /*
   * Component type declarations are pulled from the user-provided EZECS_CONFIG_FILE
   */
    
  // COMPONENT DECLARATIONS APPEAR HERE
    
  /*
   * Component type enumerator
   */
  enum ComponentTypes {
    NONE                    =  0,
    ALL                     = -1,
    ENUM_Existence          = 1 << 0,
    
    // COMPONENT TYPE ENUMERATORS APPEAR HERE
    
  };

  /*
   * numCompTypes - how many component types there are
   */
  
  // NUMBER OF COMPONENT TYPES APPEARS HERE
  
  /*
   * Component dependency field declarations
   */
  
  compMask getRequiredComps(int compType);
  compMask getDependentComps(int compType);

}

#endif //ECS_COMPONENTS_H
