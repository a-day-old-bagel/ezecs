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
#ifndef ECS_STATE_H
#define ECS_STATE_H

#include <stack>
#include <functional>
#include <vector>
#include "ecsAutoGen.h"
#include "ecsComponents.h"
#include "ecsDelegate.h"
#include "ecsKvMap.h"

namespace ecs {

  struct EntNotifyDelegate {
    Delegate<void(const entityId&, void* data)> dlgt;
    compMask likeness;
    void* data;
  };
  typedef std::vector<EntNotifyDelegate> EntNotifyDelegates;

  /**
   * Component Operation Return Values
   * are returned by all public accessors and mutators of EcsState
   */
  enum CompOpReturn {
    SUCCESS,
    NONEXISTENT_ENT,
    NONEXISTENT_COMP,
    REDUNDANT,
    PREREQ_FAIL,
    DEPEND_FAIL,
    MAX_ID_REACHED,
  };

  /**
   * EcsState - Entity Component System State
   * Within is contained all game state data pertaining to the ecs. This data takes the form of lots and lots
   * of components stored in key-value mapped structures, where the keys are entity IDs and the values are the
   * components themselves. Entities per se only exist as associations between components that share the same ID.
   */
  class State {
      // TODO: add methods to add components by copy: add[component_name](component_name& copyThis)
      /**
       * GEN_COLL_DECLS is defined in ecsComponents.h and simply wraps calls to the COMP_COLL_DECL macro.
       * The COMP_COLL_DECL (Component Collection Declaration) macros DECLARE collections of each type of component,
       * as well as methods to access and modify said collections. The following methods are generated (examples given
       * for imaginary component 'FakeComponent'):
       *
       * * * COMPONENT ADDITION * * *
       * SYNTAX:  CompOpReturn add[component_name](entityId id, [applicable constructor arguments])
       * EXAMPLE: CompOpReturn result = addFakeComponent(someId, madeUpConstructorArgument);
       * RETURNS: SUCCESS,
       *          REDUNDANT if component of same type already exists at that ID,
       *          PREREQ_FAIL if entity at that ID doesn't possess the required components for the requested component
       *                      to be made (i.e. if you tried to give it a velocity before giving it a position),
       *          NONEXISTENT_ENT if no entity exists at that ID to which to add the requested component.
       *
       * * * COMPONENT REMOVAL * * *
       * SYNTAX:  CompOpReturn rem[component_name](entityId id)
       * EXAMPLE: CompOpReturn result = remFakeComponent(someId);
       * RETURNS: SUCCESS,
       *          DEPEND_FAIL if other components at that ID depend on the component you're trying to remove,
       *          NONEXISTENT_ENT if no entity exists at that ID from which to remove the requested component,
       *          NONEXISTENT_COMP if the component you're trying to remove doesn't exist at that ID.
       *
       * * * COMPONENT RETREIVAL * * *
       * SYNTAX:  CompOpReturn get[component_name](entityId id, [component_name]** out)
       * Example: CompOpReturn result = getFakeComponent(someId, FakeComponent** myPtr);
       * RETURNS: SUCCESS,
       *          NONEXISTENT_COMP if the component you're trying to access doesn't exist at that ID.
       */
      GEN_COLL_DECLS

    public:

      /**
       * Creates a new entity (specifically an Existence component]
       * @param newId is set to the id of the newly created entity, or 0 if unsuccessful.
       * @return SUCCESS or MAX_ID_REACHED if the maximum value of the entityId type has been reached
       */
      CompOpReturn createEntity(entityId *newId);

      /**
       * Deletes all existing components from an entity except the Existence component
       * @param id The entity ID of the entity you wish to clear
       * @return SUCCESS or NONEXISENT_ENT if no entity exists at that id
       */
      CompOpReturn clearEntity(const entityId& id);

      /**
       * Deletes an entity. It's ID may be re-used later, so this 'invalidates' the ID
       * @param id The entity ID of the entity you wish to delete
       * @return any of the possible return values of remExistence given that ID (see above)
       */
      CompOpReturn deleteEntity(const entityId& id);

      /**
       * Use if you want to fire a callback whenever an entity with at least the components described by 'likeness'
       * comes into or leaved existence.
       * @param likeness The component mask describing all components necessary for an entity to trigger these callbacks
       * @param callback_add Pointer to the callback to fire when a qualifying entity appears
       * @param callback_rem Pointer to the callback to fire when such an entity ceases to qualify
       */
      void listenForLikeEntities(const compMask& likeness,
                                 EntNotifyDelegate&& additionDelegate, EntNotifyDelegate&& removalDelegate);

    private:
      entityId nextId = 0;
      std::stack<entityId> freedIds;

      /*
       * These are used by macros to generate the functions listed above (in the comment for GEN_COLL_DECLS
       */
      template<typename compType, typename ... types>
      CompOpReturn addComp(KvMap<entityId, compType>& coll, const entityId& id,
                           const EntNotifyDelegates& callbacks, const types &... args);
      template<typename compType>
      CompOpReturn remComp(KvMap<entityId, compType>& coll, const entityId& id, const EntNotifyDelegates& callbacks);
      template<typename compType>
      CompOpReturn getComp(KvMap<entityId, compType>& coll, const entityId& id, compType** out);
  };

}

#endif //ECS_STATE_H
