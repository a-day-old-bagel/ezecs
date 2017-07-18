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

#pragma once

#include <stack>
#include <functional>
#include <vector>
#include "ecsComponents.generated.hpp"
#include "delegate.hpp"
#include "ecsKvMap.hpp"

namespace ezecs {
  
  struct EntNotifyDelegate {
    rtu::Delegate<void(const entityId&, void* data)> dlgt;
    compMask likeness;
    void* data;
    inline void fire(const entityId& id) { dlgt(id, data); }
  };
  typedef std::vector<EntNotifyDelegate> EntNotifyDelegates;
  
  /**
   * Component Operation Return Values
   * are returned by all public accessors and mutators of EcsState
   */
  enum CompOpReturn {
    SUCCESS = 0,
    NONEXISTENT_ENT,
    NONEXISTENT_COMP,
    REDUNDANT,
    PREREQ_FAIL,
    DEPEND_FAIL,
    MAX_ID_REACHED,
    SOMETHING_REALLY_BAD,
  };
  
  /**
   * EcsState - Entity Component System State
   * Within is contained all game state data pertaining to the ecs. This data takes the form of lots and lots
   * of components stored in key-value mapped structures, where the keys are entity IDs and the values are the
   * components themselves. Entities per se only exist as associations between components that share the same ID.
   */
  class State {
      /**
       * Here appear collections of each type of component, as well as methods to access and modify each collection.
       * These methods are formatted as follows (examples given for imaginary component 'FakeComponent'):
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

      KvMap<entityId, Existence> comps_Existence;
      CompOpReturn getExistence(const entityId& id, Existence** out);
       
      // COMPONENT COLLECTION AND MANIPULATION METHOD DECLARATIONS APPEAR HERE

    public:

      ~State();
      
      /**
       * Creates a new entity (specifically an Existence component]
       * @param newId is set to the id of the newly created entity, or 0 if unsuccessful.
       * @return SUCCESS or MAX_ID_REACHED if the maximum value of the entityId type has been reached
       */
      CompOpReturn createEntity(entityId* newId);
      
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
       * comes into or leaves existence.
       * @param likeness The component mask describing all components necessary for an entity to trigger these callbacks
       * @param callback_add Pointer to the callback to fire when a qualifying entity appears
       * @param callback_rem Pointer to the callback to fire when such an entity ceases to qualify
       */
      void listenForLikeEntities(const compMask& likeness,
                                 EntNotifyDelegate&& additionDelegate, EntNotifyDelegate&& removalDelegate);

      /**
       * Use to get which components currently exist at an id (as a mask)
       * @param id
       * @return a component mask, or zero (NONE) if the entity does not exist
       */
      compMask getComponents(const entityId& id);

      /**
       * Deletes all entities
       */
      void clear();
    
    private:
      entityId nextId = 0;
      std::stack<entityId> freedIds;
      
      /*
       * The rest of this stuff is used by the public component collection manipulation methods
       */
      template<typename compType, typename ... types>
      inline CompOpReturn addCompNoChecks(KvMap<entityId, compType>& coll, Existence* existence,
                          const entityId& id, const EntNotifyDelegates& callbacks, const types& ... args);
      template<typename compType>
      inline CompOpReturn remCompNoChecks(KvMap<entityId, compType>& coll, Existence* existence,
                          const entityId& id, const EntNotifyDelegates& callbacks);

      template<typename compType, typename ... types>
      inline CompOpReturn addComp(KvMap<entityId, compType>& coll, const entityId& id,
                           const EntNotifyDelegates& callbacks, const types& ... args);
      template<typename compType>
      inline CompOpReturn remComp(KvMap<entityId, compType>& coll, const entityId& id,
                                  const EntNotifyDelegates& callbacks);
      template<typename compType>
      inline CompOpReturn getComp(KvMap<entityId, compType>& coll, const entityId& id, compType** out);

      inline bool shouldFireRemovalDlgt(const compMask& likeness, const compMask& current, const compMask& typeRemoved);
      inline bool shouldFireAdditionDlgt(const compMask& likeness, const compMask& current, const compMask& typeAdded);
  };
  
}
