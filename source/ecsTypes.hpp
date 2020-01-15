#pragma once

#include <cstdint>

namespace ezecs {

/*
 * "compMask" is a bit mask (or set of flags) that is used to 
 * represent a set of component types. Each bit corresponds to 
 * a single component type. "entityId" is the type used for 
 * entity IDs (surprise!).
 */
	typedef uint32_t compMask;
	typedef uint32_t entityId;

/*
 * Component base class
 * Since this is a template class to be used according to the CRTP 
 * (Curiously Recurring Template Pattern), an instance of each static 
 * member will exist for each inheritor of this class. All components 
 * must inherit from Component (this will be up to the user, who provides 
 * the configuration file).
 */
	template<typename Derived>
	struct Component {
		static compMask requiredComps;
		static compMask dependentComps;
		static compMask flag;
	};
}