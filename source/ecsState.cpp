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
#include <limits>
#include "ecsState.generated.hpp"

namespace ezecs {

  inline void* State::getCollAddr(const compMask type) {
    switch (type) {
      case EXISTENCE: return (void*)&comps_Existence;
      // COLLECTION ADDRESS GETTER CASES APPEAR HERE
      default: return NULL;
    }
  }

  CompOpReturn State::createEntity(entityId *newId) {
    entityId id;
    if (freedIds.empty()) {
      if (nextId == std::numeric_limits<entityId>::max() || ++nextId == std::numeric_limits<entityId>::max()) {
        *newId = 0; // ID 0 is not a valid id - ids start at 1. TODO: should check for id 0 in other calls?
        return MAX_ID_REACHED;
      }
      id = nextId;
    } else {
      id = freedIds.top();
      freedIds.pop();
    }
    Existence *existence = &comps_Existence[id];
    existence->turnOnFlags(Existence::flag);
    *newId = id;
    return SUCCESS;
  }

  CompOpReturn State::clearEntity(const entityId& id) {
    Existence* existence;
    CompOpReturn status = getExistence(id, &existence);
    if (status != SUCCESS) {
      return NONEXISTENT_ENT; // only fail status possible here indicates no existence component, hence no entity.
    }
    
    // A LOOP TO CLEAR ALL COMPONENTS APPEARS HERE
    
    return SUCCESS;
  }

  CompOpReturn State::deleteEntity(const entityId& id) {
    Existence* existence;
    CompOpReturn status = getExistence(id, &existence);
    if (status != SUCCESS) {
      return NONEXISTENT_ENT; // only fail status possible here indicates no existence component, hence no entity.
    }
    
    // A LOOP TO DELETE ALL COMPONENTS APPEARS HERE
    
    freedIds.push(id);
    return SUCCESS;
  }

  void State::listenForLikeEntities(const compMask& likeness,
                                    EntNotifyDelegate&& additionDelegate, EntNotifyDelegate&& removalDelegate) {
    
    // CODE TO REGISTER THE APPROPRIATE CALLBACKS APPEARS HERE
  }

  template<typename compType, typename ... types>
  CompOpReturn State::addComp(KvMap<entityId, compType>& coll, const entityId& id,
                                 const EntNotifyDelegates& callbacks, const types &... args) {
    if (comps_Existence.count(id)) {
      Existence* existence = &comps_Existence.at(id);
      if (existence->passesPrerequisitesForAddition(compType::requiredComps)) {
        if (coll.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(args...))) {
          existence->turnOnFlags(compType::flag);
          for (auto dlgt : callbacks) { // TODO: This logic seems wrong
            if ((comps_Existence.at(id).componentsPresent & dlgt.likeness) == dlgt.likeness) {
              dlgt.fire(id);
            }
          }
          return SUCCESS;
        }
        return REDUNDANT;
      }
      return PREREQ_FAIL;
    }
    return NONEXISTENT_ENT;
  }

  template<typename compType>
  CompOpReturn State::remComp(KvMap<entityId, compType>& coll, const entityId& id, const EntNotifyDelegates& callbacks) {
    if (comps_Existence.count(id)) {
      if (coll.count(id)) {
        Existence* existence = &comps_Existence.at(id);
        if (existence->passesDependenciesForRemoval(compType::dependentComps)) {
          if ((void*)&coll != (void*)&comps_Existence) {
            comps_Existence.at(id).turnOffFlags(compType::flag);
          }
          coll.erase(id);
          for (auto dlgt : callbacks) {  // TODO: This logic seems wrong
            if ((comps_Existence.at(id).componentsPresent & dlgt.likeness) != dlgt.likeness) {
              dlgt.fire(id);
            }
          }
          return SUCCESS;
        }
        return DEPEND_FAIL;
      }
      return NONEXISTENT_COMP;
    }
    return NONEXISTENT_ENT;
  }

  template<typename compType>
  CompOpReturn State::getComp(KvMap<entityId, compType> &coll, const entityId& id, compType** out) {
    if (coll.count(id)) {
      *out = &coll.at(id);
      return SUCCESS;
    }
    return NONEXISTENT_COMP;
  }

  bool State::shouldFireRemovalDlgt(const compMask& likeness, const compMask& current, const compMask& typeRemoved) {
    if (likeness & current == likeness) {
      if (likeness & ~typeRemoved != likeness) {
        return true;
      }
    }
    return false;
  }
  bool State::shouldFireAdditionDlgt(const compMask& likeness, const compMask& current, const compMask& typeAdded) {
    if (likeness & current != likeness) {
      if (likeness & (current | typeAdded) == likeness) {
        return true;
      }
    }
    return false;
  }

  /*
   * Component collection manipulation method definitions
   */
  CompOpReturn State::addExistence(const entityId& id ) { return addComp(comps_Existence, id, addCallbacks_Existence); }
  CompOpReturn State::remExistence(const entityId& id) { return remComp(comps_Existence, id, remCallbacks_Existence); }
  CompOpReturn State::getExistence(const entityId& id, Existence** out) { return getComp(comps_Existence, id, out); }
  void State::registerAddCallback_Existence (EntNotifyDelegate& dlgt) { addCallbacks_Existence.push_back(dlgt); }
  void State::registerRemCallback_Existence (EntNotifyDelegate& dlgt) { remCallbacks_Existence.push_back(dlgt); }
  
  // COMPONENT COLLECTION MANIPULATION METHOD DEFINITIONS APPEAR HERE
  
}