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

// EXTRA INCLUDES APPEAR HERE

/*
 * EZECS_COMPONENT_DEPENDENCIES( comp, ... )
 * Use this macro in your configuration file to specify which components must exist in a given entity before
 * a given component can be added to it. In other words, for a given component (the dependent component), this macro
 * specifies its prerequisite components.
 *
 * The first argument passed is the dependent component, and any following arguments are the prerequisite components
 * (in no particular order). This dependency relationship is also used to check the legality of component removal.
 *
 * For example, EZECS_COMPONENT_DEPENDENCIES( Visibility, Position, Orientation, Mesh )
 * tells the ECS that in order for a Visibility component to be created, Position, Orientation, and Mesh components must
 * already exist for that entity. Furthermore, the ECS would not allow you to remove a Position, Orientation, or Mesh
 * component from an entity possessing a Visibility component unless the Visibility component is removed first.
 *
 * In another example, EZECS_COMPONENT_DEPENDENCIES( Position ) indicates that a Position component doesn't need
 * any other component to exist prior to its creation. This is equivalent to not using the macro at all, in which case
 * it is assumed that there are no dependencies.
 *
 * It should go without saying, but use this macro at most once per component you create.
 *
 * Also, in case you're wondering where the definition of the macro is, it's resolved in the code generation step
 * (see ecsGenerator.cpp) instead of by the traditional C preprocessor.
 */
#define EZECS_COMPONENT_DEPENDENCIES( comp, ... )

namespace ezecs {

  /*
   * "compMast" is a bit mask (or set of flags) that is used to represent a set of component types. Each bit corresponds
   * to a single component type.
   * "entityId" is the type used for entity IDs (surprise!).
   */
  typedef uint32_t compMask;
  typedef uint32_t entityId;

  /*
   * Component base class
   * Since this is a template class to be used according to the CRTP (Curiously Recurring Template Pattern), an
   * instance of each static member will exist for each inheritor of this class. All components must inherit from
   * Component (this will be up to the user, who provides the configuration file).
   */
  template <typename Derived>
  struct Component {
    static compMask requiredComps;
    static compMask dependentComps;
    static compMask flag;
  };
  
  /*
   * The existence component is necessary for all entities according to the design of ezecs, and so is hardcoded in
   * here, instead of being declared and defined in the user's configuration file like all other components.
   * It includes some handy helper methods that will be available for all entities.
   */
  struct Existence : public Component<Existence> {
    compMask componentsPresent = 0;
    bool flagIsOn(int compType);
    bool passesPrerequisitesForAddition(compMask requiredComps);
    bool passesDependenciesForRemoval(compMask requiredComps);
    void turnOnFlags(compMask mask);
    void turnOffFlags(compMask mask);
  };

  /*
   * Other component type declarations are pulled from the configuration file.
   */
    
  // COMPONENT DECLARATIONS APPEAR HERE
    
  /*
   * Component type/flag enumerator
   */
  enum ComponentTypes {
    NONE =  0,
    ALL = -1,
    EXISTENCE = 1 << 0,
    // COMPONENT TYPE ENUMERATORS APPEAR HERE
  };

  /*
   * numCompTypes - how many component types there are, not counting the Existence component type.
   */
  
  // NUMBER OF COMPONENT TYPES APPEARS HERE
  
  /*
   * Component dependency getter declarations
   * "getRequiredComps" takes a component type enumerator and returns the set of component types that are prerequisite
   * for the given type's creation.
   * "getDependentComps" is similar, but instead returns the set of component types for which the given type is a
   * creation-prerequisite.
   */
  
  compMask getRequiredComps(int compType);
  compMask getDependentComps(int compType);

}

#endif //ECS_COMPONENTS_H
