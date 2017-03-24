/**
 * A Delegate in this case is a container for a stateful call to a member method or static function.
 * The goal is to be able to quickly and simply store a call to the method of any object or a call to a function
 * with state without worrying about how state is captured and whatnot.
 */

#ifndef ECS_DELEGATE_H
#define  ECS_DELEGATE_H

namespace ecs {
   /*
    * A note on type formattings: it's pretty much the same as for std::function (template arguments read like a
    * funciton signature: <return_type(param_type_0, param_type_1, ... , param_type_n)>
    * For example, for a function that returns float and takes an int and a char, a delegate type would be:
    * Delegate<float(int, char)>
    */

   /*
    * The following macro is all you need to make delegates.  Usage:
    * DELEGATE(method, obj) makes a delegate to a member method, taking the method and class instance (object), like so:
    * Given the code:
    *   int MyClass { public: int myMethod(const char a) { //do something } };
    *   MyClass myClassInstance;
    * A delegate for a call to myClassInstance.myMethod would be declared as:
    *   Delegate<int(const char)> myDelegate = DELEGATE(&MyClass::myMethod, &myClassInstance)
    * and later to invoke the delegate, just call myDelegate()
    *
    * A call to DELEGATE_NOCLASS does the same but without the object reference:
    *   void function(const char* str) { ... }
    *   Delegate<void(const char*)> delegate = DELEGATE_NOCLASS(function);
    *   delegate("a string");
    */
  #define DELEGATE(func, instRef) (NewDelegate(func).Create<func>(instRef))
  #define DELEGATE_NOCLASS(func) (NewDelegate_NoClass(func).CreateForFunction<func>()) // delegate to function

  // And here are come all the guts...
  template<typename returnType, typename... params>
  class Delegate; // this advance declaration allows for the templating ahead.
  // main Delegate class definition
  template<typename returnType, typename... params>
  class Delegate<returnType(params...)> {
      typedef returnType (*PtrToFunc)(void* callee, params...);
    public:
      Delegate() {}
      Delegate(void* callee, PtrToFunc function) : calleePtr(callee) , callbackFuncPtr(function) {}
      returnType operator()(params... args) const {
        return (*callbackFuncPtr)(calleePtr, args...);
      }
      bool operator==(const Delegate& rhs) const {
        return (calleePtr == rhs.calleePtr) && (callbackFuncPtr == rhs.callbackFuncPtr);
      }
    private:
      void* calleePtr;
      PtrToFunc callbackFuncPtr;
  };

  // Delegate spawner makes delegates, handles all the casting required for delegate operation.
  template<typename className, typename returnType, typename... params>
  struct DelegateSpawner {
    template<returnType (className::*func)(params...)>
    static returnType MethodCaller(void* o, params... xs){
      return (static_cast<className*>(o)->*func)(xs...);
    }
    template <returnType (*classFuncPtr)(params...)>
    static returnType FunctionCaller(void*, params... xs){
      return (classFuncPtr)(xs...);
    }
    template<returnType (className::*func)(params...)>
    inline static Delegate<returnType(params...)> Create(className* o){
      return Delegate<returnType(params...)>(o, &DelegateSpawner::MethodCaller<func>);
    }
    template<returnType (*classFuncPtr)(params...)>
    inline static Delegate<returnType(params...)> CreateForFunction(){
      return Delegate<returnType(params...)>(0L, &DelegateSpawner::FunctionCaller<classFuncPtr>);
    }
  };

  // helper function that returns delegate spawner of member method delegates
  template<typename className, typename returnType, typename... params>
  DelegateSpawner<className, returnType, params... > NewDelegate(returnType (className::*)(params...)){
    return DelegateSpawner<className, returnType, params...>();
  }
  class noType{}; // noType class used in the function below
  // helper function that returns delegate spawner of function delegates
  template<typename returnType, typename... params>
  DelegateSpawner<noType, returnType, params... > NewDelegate_NoClass(returnType (*TFnctPtr)(params...)){
    return DelegateSpawner<noType, returnType, params...>();
  }
}

#endif //ECS_DELEGATE_H