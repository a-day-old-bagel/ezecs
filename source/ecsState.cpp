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
#include "ecsState.h"

namespace ecs {
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
    GEN_CLEAR_ENT_LOOP_DEFN(ALL_COMPS)
    return SUCCESS;
  }

  CompOpReturn State::deleteEntity(const entityId& id) {
    Existence* existence;
    CompOpReturn status = getExistence(id, &existence);
    if (status != SUCCESS) {
      return NONEXISTENT_ENT; // only fail status possible here indicates no existence component, hence no entity.
    }
    GEN_DEL_ENT_LOOP_DEFN(ALL_COMPS)
    freedIds.push(id);
    return SUCCESS;
  }

  void State::listenForLikeEntities(const compMask& likeness,
                                    EntNotifyDelegate&& additionDelegate, EntNotifyDelegate&& removalDelegate) {
    GEN_LISTEN_FOR_LIKE_ENTITIES_INTERNALS(ALL_COMPS)
  }

  template<typename compType, typename ... types>
  CompOpReturn State::addComp(KvMap<entityId, compType>& coll, const entityId& id,
                                 const EntNotifyDelegates& callbacks, const types &... args) {
    if (comps_Existence.count(id)) {
      Existence* existence = &comps_Existence.at(id);
      if (existence->passesPrerequisitesForAddition(compType::requiredComps)) {
        if (coll.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(args...))) {
          existence->turnOnFlags(compType::flag);
          for (auto dlgt : callbacks) {
            if ((comps_Existence.at(id).componentsPresent & dlgt.likeness) == dlgt.likeness) {
              dlgt.dlgt(id, dlgt.data);
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
    if (coll.count(id)) {
      if (comps_Existence.count(id)) {
        Existence* existence = &comps_Existence.at(id);
        if (existence->passesDependenciesForRemoval(compType::dependentComps)) {
          if ((void*)&coll != (void*)&comps_Existence) {
            comps_Existence.at(id).turnOffFlags(compType::flag);
          }
          coll.erase(id);
          for (auto dlgt : callbacks) {
            if ((comps_Existence.at(id).componentsPresent & dlgt.likeness) != dlgt.likeness) {
              dlgt.dlgt(id, dlgt.data);
            }
          }
          return SUCCESS;
        }
        return DEPEND_FAIL;
      }
      return NONEXISTENT_ENT;
    }
    return NONEXISTENT_COMP;
  }

  template<typename compType>
  CompOpReturn State::getComp(KvMap<entityId, compType> &coll, const entityId& id, compType** out) {
    if (coll.count(id)) {
      *out = &coll.at(id);
      return SUCCESS;
    }
    return NONEXISTENT_COMP;
  }

  /*
   * GEN_COLL_DEFNS wraps calls to the GEN_COMP_COLL_DEFN macro as defined in ecsComponents.h.
   * The GEN_COMP_COLL_DEFN macros DEFINE collections of each type of component and methods to access and modify them.
   */
  GEN_COLL_DEFNS
}