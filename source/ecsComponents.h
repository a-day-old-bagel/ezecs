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

#include "ecsAutoGen.h"
#include "ecsDelegate.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class btCollisionShape;
class btDefaultMotionState;
class btRigidBody;

namespace ecs {

  typedef uint32_t compMask;
  typedef uint32_t entityId;

  template <typename Derived>
  struct Component {
    static compMask requiredComps;
    static compMask dependentComps;
    static compMask flag;
  };

  /*
   * The following are component type declarations.
   * TODO: Declare new components here and be sure to define 'SIG_[component_type]' as the constructor parameter types.
   * NOTE: Just follow the examples of the others.
   */
  #define SIG_Existence
  struct Existence : public Component<Existence> {
    compMask componentsPresent = 0;
    bool flagIsOn(int compType);
    bool passesPrerequisitesForAddition(compMask mask);
    bool passesDependenciesForRemoval(compMask mask);
    void turnOnFlags(compMask mask);
    void turnOffFlags(compMask mask);
  };
  #define SIG_Position glm::vec3
  struct Position : public Component<Position> {
    glm::vec3 vec, lastVec;
    Position(glm::vec3 vec);
    glm::vec3 getVec(float alpha);
  };
  #define SIG_Scale glm::vec3
  struct Scale : public Component<Scale> {
    glm::vec3 vec, lastVec;
    Scale(glm::vec3 vec);
  };
  #define SIG_ScalarMultFunc Delegate<glm::vec3(const glm::vec3&, uint32_t time)>
  struct ScalarMultFunc : public Component<ScalarMultFunc> {
    Delegate<glm::vec3(const glm::vec3&, uint32_t time)> multByFuncOfTime;
    ScalarMultFunc(Delegate<glm::vec3(const glm::vec3&, uint32_t time)> func);
  };
  #define SIG_Orientation glm::quat
  struct Orientation : public Component<Orientation> {
    glm::quat quat, lastQuat;
    Orientation(glm::quat quat);
    glm::quat getQuat(float alpha);
  };
  #define SIG_Perspective float, float, float
  struct Perspective : public Component<Perspective> {
    float fovy, prevFovy,
          near, far;
    Perspective(float fovy, float near, float far);
  };
  #define SIG_WasdControls entityId, WasdControls::Style
  struct WasdControls : public Component<WasdControls> {
    enum Style {
      ROTATE_ALL_AXES, ROTATE_ABOUT_Z
    };
    glm::vec3 accel;
    entityId orientationProxy;
    int style;
    WasdControls(entityId orientationProxy, Style style);
  };
  #define SIG_MouseControls bool, bool
  struct MouseControls : public Component<MouseControls> {
    bool invertedX, invertedY;
    MouseControls(bool invertedX, bool invertedY);
  };
  #define SIG_Physics float, void*, Physics::Geometry
  struct Physics : public Component<Physics> {
    enum Geometry {
      NONE, PLANE, SPHERE, MESH
    };
    int geom;
    float mass;
    btCollisionShape* shape;
    btRigidBody* rigidBody;
    void* geomInitData;
    Physics(float mass, void* geomData, Geometry geom);
  };

  /*
   * TODO: Add an entry to the end of each of the three directives below for any new component types you create.
   * (follow examples of others)
   */
  #define ALL_COMPS \
    Existence,      \
    Position,       \
    Scale,          \
    ScalarMultFunc, \
    Orientation,    \
    Perspective,    \
    WasdControls,   \
    MouseControls,  \
    Physics

  #define GEN_COLL_DECLS \
    GEN_COMP_COLL_DECL(Existence)     \
    GEN_COMP_COLL_DECL(Position)      \
    GEN_COMP_COLL_DECL(Scale)         \
    GEN_COMP_COLL_DECL(ScalarMultFunc)\
    GEN_COMP_COLL_DECL(Orientation)   \
    GEN_COMP_COLL_DECL(Perspective)   \
    GEN_COMP_COLL_DECL(WasdControls)  \
    GEN_COMP_COLL_DECL(MouseControls) \
    GEN_COMP_COLL_DECL(Physics)

  #define GEN_COLL_DEFNS \
    GEN_COMP_COLL_DEFN(Existence)     \
    GEN_COMP_COLL_DEFN(Position)      \
    GEN_COMP_COLL_DEFN(Scale)         \
    GEN_COMP_COLL_DEFN(ScalarMultFunc)\
    GEN_COMP_COLL_DEFN(Orientation)   \
    GEN_COMP_COLL_DEFN(Perspective)   \
    GEN_COMP_COLL_DEFN(WasdControls)  \
    GEN_COMP_COLL_DEFN(MouseControls) \
    GEN_COMP_COLL_DEFN(Physics)

  /*
   * This macro does the following:
   * generates bit flag enumerations for all component types
   * declares and defines uint8_t numCompTypes as the number of component types total
   * declares these functions:
   */
  GEN_COMP_DECLS(ALL_COMPS)

  compMask getRequiredComps(int compType);
  compMask getDependentComps(int compType);

}

#endif //ECS_COMPONENTS_H
