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
#include "ecsComponents.h"

namespace ecs {

  /*
   * The following macros describe the dependency relationships between components.The first block enumerates for
   * a given component what other components are required for it to exist. For example, it makes no sense for an
   * entity to posses linear velocity without first having a position. This relationship is important when you're
   * adding new components to the world. Notice that all components list the existence component as a requirement.
   *
   * The second set of macros describe the inverse relationships, or for a given component, which other components
   * list it as a required component. This relationship is examined upon the deletion of a component. Notice that
   * the existence component is a lists 'ALL' (minus itself) as its dependents.
   *
   * TODO: Add GEN_COMP_DEFN_REQD and GEN_COMP_DEFN_DEPN entries below for any new component types you create.
   * The generated format for any component enumerator is 'ENUM_[component_type].' As seen below, the ALL and NONE
   * enumerators also exist. Since these are bit flags, you can probably guess that NONE is zero and ALL is
   * unsigned -1, or in other words, ALL has all the bits turned on.
   */
  GEN_COMP_DEFN_REQD(Existence, NONE);
  GEN_COMP_DEFN_REQD(Position, ENUM_Existence);
  GEN_COMP_DEFN_REQD(Scale, ENUM_Existence);
  GEN_COMP_DEFN_REQD(ScalarMultFunc, ENUM_Existence | ENUM_Scale);
  GEN_COMP_DEFN_REQD(Orientation, ENUM_Existence);
  GEN_COMP_DEFN_REQD(Perspective, ENUM_Existence | ENUM_Position | ENUM_Orientation);
  GEN_COMP_DEFN_REQD(WasdControls, ENUM_Existence | ENUM_Orientation);
  GEN_COMP_DEFN_REQD(MouseControls, ENUM_Existence | ENUM_Orientation);
  GEN_COMP_DEFN_REQD(Physics, ENUM_Existence | ENUM_Position | ENUM_Orientation);

  GEN_COMP_DEFN_DEPN(Existence, ALL & ~ENUM_Existence);
  GEN_COMP_DEFN_DEPN(Position, ENUM_Perspective | ENUM_Physics);
  GEN_COMP_DEFN_DEPN(Scale, ENUM_ScalarMultFunc);
  GEN_COMP_DEFN_DEPN(ScalarMultFunc, NONE);
  GEN_COMP_DEFN_DEPN(Orientation, ENUM_Perspective | ENUM_Physics | ENUM_MouseControls | ENUM_WasdControls);
  GEN_COMP_DEFN_DEPN(Perspective, NONE);
  GEN_COMP_DEFN_DEPN(WasdControls, NONE);
  GEN_COMP_DEFN_DEPN(MouseControls, NONE);
  GEN_COMP_DEFN_DEPN(Physics, NONE);

  /*
   * The following area is for the definitions of any component methods you create. Make sure that constructor
   * parameters match the 'SIG_[component_type]' define you provided with the declaration of the component.
   * TODO: Add any and all component member method definitions here.
   * NOTE: Generally there shouldn't be many methods except the constructor. Game logic ought to go in the systems..
   * Sometimes its convenient to put helper methods in some components, however (like the interpolating getters in
   * the Position and Orientation components), so those are probably ok.
   */
  bool Existence::flagIsOn(int compType) {
    return (compType & componentsPresent) != NONE;
  }
  bool Existence::passesPrerequisitesForAddition(compMask mask) {
    return (mask & componentsPresent) == mask;
  }
  bool Existence::passesDependenciesForRemoval(compMask mask) {
    return (mask & componentsPresent) == NONE;
  }
  void Existence::turnOnFlags(compMask mask) {
    componentsPresent |= mask;
  }
  void Existence::turnOffFlags(compMask mask) {
    componentsPresent &= ~mask;
  }
  Position::Position(glm::vec3 vec) : vec(vec), lastVec(vec) {}
  glm::vec3 Position::getVec(float alpha) {
    return glm::mix(lastVec, vec, alpha);
  }
  Scale::Scale(glm::vec3 vec) : vec(vec), lastVec(vec) { }
  ScalarMultFunc::ScalarMultFunc(Delegate<glm::vec3(const glm::vec3 &, uint32_t)> func) : multByFuncOfTime(func) { }
  Orientation::Orientation(glm::quat quat) : quat(quat), lastQuat(quat) {}
  glm::quat Orientation::getQuat(float alpha) {
    return glm::slerp(lastQuat, quat, alpha);
  }
  Perspective::Perspective(float fovy, float near, float far)
      : fovy(fovy), prevFovy(fovy), near(near), far(far) {}
  WasdControls::WasdControls(entityId orientationProxy, Style style) : orientationProxy(orientationProxy), style(style) { }
  MouseControls::MouseControls(bool invertedX, bool invertedY) : invertedX(invertedX), invertedY(invertedY) { }
  Physics::Physics(float mass, void* geomData, Geometry geom) : geom(geom), mass(mass), geomInitData(geomData) { }

  /*
   * This macro defines these function:
   * compMask getRequiredComps(int compType);
   * compMask getDependentComps(int compType);
   */
  GEN_COMP_DEFNS(ALL_COMPS);
}
