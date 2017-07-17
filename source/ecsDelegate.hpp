/**
 * A Delegate in this case is a container for a stateful call to a member method or static function.
 * The goal is to be able to quickly and simply store a call to the method of any object or a call to a function
 * with state without worrying about how state is captured and whatnot.
 */
#pragma once

namespace ezecs {
   /*
    * A note on type formattings: it's pretty much the same as for std::function (template arguments read like a
    * funciton signature: <return_type(param_type_0, param_type_1, ... , param_type_n)>
    * For example, for a function that returns float and takes an int and a char, a delegate type would be:
    * EcsDelegate<float(int, char)>
    *
    * If you wonder why I don't just use std::function, one reason is because I've been using this since before
    * c++11 and I'm used to it. There doesn't seem to be a good motivation to change atm.
    */

   /*
    * The following macro is all you need to make delegates.  Usage:
    * ECS_DELEGATE(method, obj) makes a delegate to a member method, taking the method and class instance (object),
    * like so:
    * Given the code:
    *   int MyClass { public: int myMethod(const char a) { //do something } };
    *   MyClass myClassInstance;
    * A delegate for a call to myClassInstance.myMethod would be declared as:
    *   EcsDelegate<int(const char)> myDelegate = ECS_DELEGATE(&MyClass::myMethod, &myClassInstance)
    * and later to invoke the delegate, just call myDelegate()
    *
    * A call to ECS_DELEGATE_NOCLASS does the same but without the object reference:
    *   void function(const char* str) { ... }
    *   EcsDelegate<void(const char*)> delegate = ECS_DELEGATE_NOCLASS(function);
    *   delegate("a string");
    */
  #ifndef ECS_DELEGATE
    #define ECS_DELEGATE(func, instRef) (NewEcsDelegate(func).Create<func>(instRef)) // deletage to member method
  #endif
  #ifndef ECS_DELEGATE_NOCLASS
    #define ECS_DELEGATE_NOCLASS(func) (NewEcsDelegate_NoClass(func).CreateForFunction<func>()) // delegate to function
  #endif

  // And here are come all the guts...
  template<typename returnType, typename... params>
  class EcsDelegate; // this advance declaration allows for the templating ahead.
  // main EcsDelegate class definition
  template<typename returnType, typename... params>
  class EcsDelegate<returnType(params...)> {
      typedef returnType (*PtrToFunc)(void* callee, params...);
    public:
      EcsDelegate() {}
      EcsDelegate(void* callee, PtrToFunc function) : calleePtr(callee) , callbackFuncPtr(function) {}
      returnType operator()(params... args) const {
        return (*callbackFuncPtr)(calleePtr, args...);
      }
      bool operator==(const EcsDelegate& rhs) const {
        return (calleePtr == rhs.calleePtr) && (callbackFuncPtr == rhs.callbackFuncPtr);
      }
    private:
      void* calleePtr;
      PtrToFunc callbackFuncPtr;
  };

  // EcsDelegate spawner makes delegates, handles all the casting required for delegate operation.
  template<typename className, typename returnType, typename... params>
  struct EcsDelegateSpawner {
    template<returnType (className::*func)(params...)>
    static returnType MethodCaller(void* o, params... xs){
      return (static_cast<className*>(o)->*func)(xs...);
    }
    template <returnType (*classFuncPtr)(params...)>
    static returnType FunctionCaller(void*, params... xs){
      return (classFuncPtr)(xs...);
    }
    template<returnType (className::*func)(params...)>
    inline static EcsDelegate<returnType(params...)> Create(className* o){
      return EcsDelegate<returnType(params...)>(o, &EcsDelegateSpawner::MethodCaller<func>);
    }
    template<returnType (*classFuncPtr)(params...)>
    inline static EcsDelegate<returnType(params...)> CreateForFunction(){
      return EcsDelegate<returnType(params...)>(0L, &EcsDelegateSpawner::FunctionCaller<classFuncPtr>);
    }
  };

  // helper function that returns delegate spawner of member method delegates
  template<typename className, typename returnType, typename... params>
  EcsDelegateSpawner<className, returnType, params... > NewEcsDelegate(returnType (className::*)(params...)){
    return EcsDelegateSpawner<className, returnType, params...>();
  }
  class noType{}; // noType class used in the function below
  // helper function that returns delegate spawner of function delegates
  template<typename returnType, typename... params>
  EcsDelegateSpawner<noType, returnType, params... > NewEcsDelegate_NoClass(returnType (*TFnctPtr)(params...)){
    return EcsDelegateSpawner<noType, returnType, params...>();
  }
}
