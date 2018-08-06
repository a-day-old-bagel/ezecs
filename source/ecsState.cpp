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

  CompOpReturn State::createEntity(entityId *newId) {
    entityId id;
    if (freedIds.empty()) {
      if (nextId == std::numeric_limits<entityId>::max() || ++nextId == std::numeric_limits<entityId>::max()) {
        if (newId) {
          *newId = 0; // ID 0 is not a valid id - ids start at 1.
        }
        return MAX_ID_REACHED;
      }
      id = nextId;
    } else {
      id = freedIds.top();
      freedIds.pop();
    }
    Existence *existence = &comps_Existence[id];
    existence->turnOnFlags(Existence::flag);
    if (newId) {
      *newId = id;
    }
    return SUCCESS;
  }

  CompOpReturn State::clearEntity(const entityId& id) {
    Existence* existence;
    CompOpReturn status = getExistence(id, &existence);
    if (status != SUCCESS) {
      return NONEXISTENT_ENT; // only fail status possible here indicates no existence component, hence no entity.
    }

    // A LOOP TO CLEAR ALL COMPONENTS APPEARS HERE

    if (existence->componentsPresent != Existence::flag) {
      return SOMETHING_REALLY_BAD;
    }
    return SUCCESS;
  }

  CompOpReturn State::deleteEntity(const entityId& id) {
    CompOpReturn cleared = clearEntity(id);
    if (cleared != SUCCESS) {
      return cleared;
    }
    comps_Existence.erase(id);
    freedIds.push(id);
    return SUCCESS;
  }

  void State::listenForLikeEntities(const compMask& likeness,
                                    EntNotifyDelegate&& additionDelegate, EntNotifyDelegate&& removalDelegate)
  {
    // CODE TO REGISTER THE APPROPRIATE CALLBACKS APPEARS HERE
  }

  compMask State::getComponents(const entityId& id) {
    Existence* existence;
    if (getExistence(id, &existence) == SUCCESS) {
      return existence->componentsPresent;
    }
    return 0; // does not return compOpReturn, so a 0 indicates no components (even existence) present.
  }

  entityId State::getNextId() {
    return nextId;
  }

  void State::clear() {
    std::vector<entityId> idsToErase;
    for (auto pair : comps_Existence) {
      idsToErase.push_back(pair.first);
    }
    for (auto id : idsToErase) {
      deleteEntity(id);
    }
  }

  template<typename compType, typename ... types>
  inline CompOpReturn State::addCompNoChecks(KvMap<entityId, compType>& coll, Existence* existence,
                             const entityId& id, const EntNotifyDelegates& callbacks, const types& ... args)
  {
    if (coll.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(args...))) {
      for (auto dlgt : callbacks) {
        if (shouldFireAdditionDlgt(dlgt.likeness, existence->componentsPresent, compType::flag)) {
          dlgt.fire(id);
        }
      }
      existence->turnOnFlags(compType::flag);
      return SUCCESS;
    }
    return REDUNDANT;
  }
  template<typename compType>
  inline CompOpReturn State::remCompNoChecks(KvMap<entityId, compType>& coll, Existence* existence,
                                                      const entityId& id, const EntNotifyDelegates& callbacks)
  {
    for (auto dlgt : callbacks) {
      if (shouldFireRemovalDlgt(dlgt.likeness, existence->componentsPresent, compType::flag)) {
        dlgt.fire(id);
      }
    }
    coll.erase(id);
    existence->turnOffFlags(compType::flag);
    return SUCCESS;
  }

  template<typename compType, typename ... types>
  inline CompOpReturn State::addComp(KvMap<entityId, compType>& coll, const entityId& id,
                                 const EntNotifyDelegates& callbacks, const types &... args)
  {
    if (comps_Existence.count(id)) {
      Existence* existence = &comps_Existence.at(id);
      if (existence->passesPrerequisitesForAddition(compType::requiredComps)) {
        return addCompNoChecks(coll, existence, id, callbacks, args...);
      }
      return PREREQ_FAIL;
    }
    return NONEXISTENT_ENT;
  }

  template<typename compType>
  inline CompOpReturn State::remComp(KvMap<entityId, compType>& coll, const entityId& id,
                                     const EntNotifyDelegates& callbacks)
  {
    if (comps_Existence.count(id)) {
      if (coll.count(id)) {
        Existence* existence = &comps_Existence.at(id);
        if (existence->passesDependenciesForRemoval(compType::dependentComps)) {
          return remCompNoChecks(coll, existence, id, callbacks);
        }
        return DEPEND_FAIL;
      }
      return NONEXISTENT_COMP;
    }
    return NONEXISTENT_ENT;
  }

  template<typename compType>
  inline CompOpReturn State::getComp(KvMap<entityId, compType> &coll, const entityId& id, compType** out) {
    if (coll.count(id)) {
      *out = &coll.at(id);
      return SUCCESS;
    }
    *out = NULL; // hopefully a null pointer exception will catch some bugs if somebody uses this wrong.
    return NONEXISTENT_COMP;
  }

  inline bool State::shouldFireRemovalDlgt(const compMask& likeness, const compMask& current,
                                           const compMask& typeRemoved)
  {
    if ( (likeness & current) == likeness ) {
      if ( (likeness & ~typeRemoved) != likeness) {
        return true;
      }
    }
    return false;
  }
  inline bool State::shouldFireAdditionDlgt(const compMask& likeness, const compMask& current,
                                            const compMask& typeAdded)
  {
    if ( (likeness & current) != likeness) {
      if ( (likeness & (current | typeAdded)) == likeness) {
        return true;
      }
    }
    return false;
  }

  /*
   * Component collection manipulation method definitions
   */
  CompOpReturn State::getExistence(const entityId& id, Existence** out) { return getComp(comps_Existence, id, out); }

  // COMPONENT COLLECTION MANIPULATION METHOD DEFINITIONS APPEAR HERE

}
