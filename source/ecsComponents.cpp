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
#include "ecsComponents.generated.hpp"

namespace ezecs {

  /*
   * The following fields describe the dependency relationships between components. The 'requiredComps' field of a given
   * component enumerates which other components (if any) are required for it to exist. For example, it makes no sense
   * for an entity to posses linear velocity without first having a position. This relationship is important when you're
   * adding new components to the world. Notice that all components list the existence component as a requirement.
   *
   * The 'dependentComps' field describe the inverse relationships, or for a given component, which other components
   * list it as a required component. This relationship is examined upon the deletion of a component. Notice that
   * the existence component is a lists 'ALL' (minus itself) as its dependents.
   *
   * The generated format for any component enumerator is 'ENUM_[component_type].' ALL and NONE
   * enumerators also exist. Since these are bit flags, you can probably guess that NONE is zero and ALL is
   * unsigned -1, or in other words, ALL has all the bits turned on.
   */
  
  template<> compMask Component<Existence>::requiredComps = NONE;
  template<> compMask Component<Existence>::dependentComps = ALL & ~EXISTENCE;
  template<> compMask Component<Existence>::flag = EXISTENCE;
  // COMPONENT DEPENDENCY FIELD DEFINITIONS APPEAR HERE
  
  /*
   * Existence component method definitions
   */
  bool Existence::flagIsOn(int compType) {
    return (compType & componentsPresent) != NONE;
  }
  bool Existence::passesPrerequisitesForAddition(compMask requiredComps) {
    return (requiredComps & componentsPresent) == requiredComps;
  }
  bool Existence::passesDependenciesForRemoval(compMask requiredComps) {
    return (requiredComps & componentsPresent) == NONE;
  }
  void Existence::turnOnFlags(compMask mask) {
    componentsPresent |= mask;
  }
  void Existence::turnOffFlags(compMask mask) {
    componentsPresent &= ~mask;
  }
  
  /*
   * The following area is for the definitions of any component methods you create.
   *
   * NOTE: Generally there shouldn't be many methods except the constructor. Game logic ought to go in the systems..
   * Sometimes its convenient to put helper methods in some components, however (like the interpolating getters in
   * the Position and Orientation components), so those are probably ok.
   */
 
  // COMPONENT METHOD DEFINITIONS APPEAR HERE

  /*
   * compMask getRequiredComps(int compType);
   * compMask getDependentComps(int compType);
   */
  compMask getRequiredComps(int compType) {
    switch(compType) {
      case EXISTENCE: return Existence::requiredComps;
      // COMPONENT REQUIREMENTS GETTER CASES APPEAR HERE
      default: return ALL;
    }
  }
  compMask getDependentComps(int compType) {
    switch(compType) {
      case EXISTENCE: return Existence::dependentComps;
      // COMPONENT DEPENDENTS GETTER CASES APPEAR HERE
      default: return ALL;
    }
  }
}
