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

/**
 * This file contains a bunch of scary macros that allow for the automatic generation of code within
 * ecsComponents.h and .cpp and ecsState.h and .cpp
 *
 * It is not advised that you try to follow them all the way through unless you have intimate knowledge of
 * variadic macros and tricks like for-each macros.
 *
 * A much more friendly interface for editing components is found in ecsComponents.h and .cpp
 */
#ifndef ECS_AUTOGEN_H
#define ECS_AUTOGEN_H

#include <stdint.h>

// MSVC workaround for their failure to handle __VA_ARGS__ in a reasonable way
#define EXPAND(x) x

// Provides variadic argument count
#define _GET_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, \
                  _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _N, ...) _N

#define GET_NUM_ARGS(...) EXPAND(_GET_ARG_N(__VA_ARGS__, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20\
                          19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))

// The individual iterations of the for each (see DO_FOR_EACH below)
#define _i0(action, ...)
#define _i1(action, arg)      action(arg, 0)
#define _i2(action, arg, ...) action(arg, 1) EXPAND(_i1(action, __VA_ARGS__))
#define _i3(action, arg, ...) action(arg, 2) EXPAND(_i2(action, __VA_ARGS__))
#define _i4(action, arg, ...) action(arg, 3) EXPAND(_i3(action, __VA_ARGS__))
#define _i5(action, arg, ...) action(arg, 4) EXPAND(_i4(action, __VA_ARGS__))
#define _i6(action, arg, ...) action(arg, 5) EXPAND(_i5(action, __VA_ARGS__))
#define _i7(action, arg, ...) action(arg, 6) EXPAND(_i6(action, __VA_ARGS__))
#define _i8(action, arg, ...) action(arg, 7) EXPAND(_i7(action, __VA_ARGS__))
#define _i9(action, arg, ...) action(arg, 8) EXPAND(_i8(action, __VA_ARGS__))
#define _i10(action, arg, ...) action(arg, 9) EXPAND(_i9(action, __VA_ARGS__))
#define _i11(action, arg, ...) action(arg, 10) EXPAND(_i10(action, __VA_ARGS__))
#define _i12(action, arg, ...) action(arg, 11) EXPAND(_i11(action, __VA_ARGS__))
#define _i13(action, arg, ...) action(arg, 12) EXPAND(_i12(action, __VA_ARGS__))
#define _i14(action, arg, ...) action(arg, 13) EXPAND(_i13(action, __VA_ARGS__))
#define _i15(action, arg, ...) action(arg, 14) EXPAND(_i14(action, __VA_ARGS__))
#define _i16(action, arg, ...) action(arg, 15) EXPAND(_i15(action, __VA_ARGS__))
#define _i17(action, arg, ...) action(arg, 16) EXPAND(_i16(action, __VA_ARGS__))
#define _i18(action, arg, ...) action(arg, 17) EXPAND(_i17(action, __VA_ARGS__))
#define _i19(action, arg, ...) action(arg, 18) EXPAND(_i18(action, __VA_ARGS__))
#define _i20(action, arg, ...) action(arg, 19) EXPAND(_i19(action, __VA_ARGS__))
#define _i21(action, arg, ...) action(arg, 20) EXPAND(_i20(action, __VA_ARGS__))
#define _i22(action, arg, ...) action(arg, 21) EXPAND(_i21(action, __VA_ARGS__))
#define _i23(action, arg, ...) action(arg, 22) EXPAND(_i22(action, __VA_ARGS__))
#define _i24(action, arg, ...) action(arg, 23) EXPAND(_i23(action, __VA_ARGS__))
#define _i25(action, arg, ...) action(arg, 24) EXPAND(_i24(action, __VA_ARGS__))
#define _i26(action, arg, ...) action(arg, 25) EXPAND(_i25(action, __VA_ARGS__))
#define _i27(action, arg, ...) action(arg, 26) EXPAND(_i26(action, __VA_ARGS__))
#define _i28(action, arg, ...) action(arg, 27) EXPAND(_i27(action, __VA_ARGS__))
#define _i29(action, arg, ...) action(arg, 28) EXPAND(_i28(action, __VA_ARGS__))
#define _i30(action, arg, ...) action(arg, 29) EXPAND(_i29(action, __VA_ARGS__))
#define _i31(action, arg, ...) action(arg, 30) EXPAND(_i30(action, __VA_ARGS__))
#define _i32(action, arg, ...) action(arg, 31) EXPAND(_i31(action, __VA_ARGS__))

/**
 * Variadic Macro For-each. 'action' must be a macro which takes two arguments,
 * the second one being an iteration counter.
 * 'action' may be followed by up to 32 comma separated items
 * which will consecutively be given to the 'action' macro.
 */
#define DO_FOR_EACH(action, ...) \
    EXPAND(_GET_ARG_N(placeholder, ##__VA_ARGS__, \
    _i32, _i31, _i30, _i29, _i28, _i27, _i26, _i25, _i24, _i23, _i22, _i21, _i20, _i19, _i18, _i17, _i16, \
    _i15, _i14, _i13, _i12, _i11, _i10, _i9, _i8, _i7, _i6, _i5, _i4, _i3, _i2, _i1, _i0)) \
    (action, ##__VA_ARGS__)

// Used in DO_FOR_EACH_PAIR below.
#define _a0(action, ...)
#define _a2(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a0(action, __VA_ARGS__))
#define _a4(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a2(action, __VA_ARGS__))
#define _a6(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a4(action, __VA_ARGS__))
#define _a8(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a6(action, __VA_ARGS__))
#define _a10(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a8(action, __VA_ARGS__))
#define _a12(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a10(action, __VA_ARGS__))
#define _a14(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a12(action, __VA_ARGS__))
#define _a16(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a14(action, __VA_ARGS__))
#define _a18(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a16(action, __VA_ARGS__))
#define _a20(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a18(action, __VA_ARGS__))
#define _a22(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a20(action, __VA_ARGS__))
#define _a24(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a22(action, __VA_ARGS__))
#define _a26(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a24(action, __VA_ARGS__))
#define _a28(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a26(action, __VA_ARGS__))
#define _a30(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a28(action, __VA_ARGS__))
#define _a32(action, arg0, arg1, ...) action(arg0, arg1) EXPAND(_a30(action, __VA_ARGS__))

/**
 * Also iterates through variadic macro, but does so two at a time.
 * Accepts action, therefore, that acts upon two arguments.
 * If an odd number of variadic argments are provided, the last is neglected.
 */
#define DO_FOR_EACH_PAIR(action, ...) \
    _GET_ARG_N(placeholder, ##__VA_ARGS__, \
    _a32, _a30, _a30, _a28, _a28, _a26, _a26, _a24, _a24, _a22, _a22, _a20, _a20, _a18, _a18, _a16, _a16, \
    _a14, _a14, _a12, _a12, _a10, _a10, _a8, _a8, _a6, _a6, _a4, _a4, _a2, _a2, _a0, _a0) \
    (action, ##__VA_ARGS__)

#define _GEN_COMP_ENUM(comp, i) ENUM_##comp = 1 << i,
#define GEN_COMP_DECLS(...) enum ComponentTypes { NONE = 0, ALL = -1, EXPAND(DO_FOR_EACH(_GEN_COMP_ENUM, __VA_ARGS__)) }; \
                            const uint8_t numCompTypes = EXPAND(GET_NUM_ARGS(__VA_ARGS__));


#define _GEN_COMP_CASE_REQD(comp, i) case ENUM_##comp: return comp::requiredComps;
#define _GEN_COMP_CASE_DEPN(comp, i) case ENUM_##comp: return comp::dependentComps;
#define GEN_COMP_DEFNS(...) compMask getRequiredComps(int compType) { switch(compType) { \
                            DO_FOR_EACH(_GEN_COMP_CASE_REQD, __VA_ARGS__) default: return ALL; } } \
                            compMask getDependentComps(int compType) { switch(compType) { \
                            DO_FOR_EACH(_GEN_COMP_CASE_DEPN, __VA_ARGS__) default: return ALL; } }

#define _GEN_CLEAR_ENT(comp, i) \
if (ENUM_##comp != ENUM_Existence) { \
  if (existence->flagIsOn(ENUM_##comp)) { \
    existence->turnOffFlags(ENUM_##comp); \
    for (auto dlgt : remCallbacks_##comp) { \
      if ((comps_Existence.at(id).componentsPresent & dlgt.likeness) != dlgt.likeness) { \
        dlgt.dlgt(id, dlgt.data); \
      } \
    } \
  } \
  if (comps_##comp.count(id)) { \
    comps_##comp.erase(id); \
  } \
}
#define GEN_CLEAR_ENT_LOOP_DEFN(...) DO_FOR_EACH(_GEN_CLEAR_ENT, __VA_ARGS__)

#define _GEN_DEL_ENT(comp, i) \
for (auto dlgt : remCallbacks_##comp) { /* give that id to all removal delegates no matter what */\
  dlgt.dlgt(id, dlgt.data); \
} \
if (comps_##comp.count(id)) { /* delete all component data for that id if it exists */\
  comps_##comp.erase(id); \
}
#define GEN_DEL_ENT_LOOP_DEFN(...) DO_FOR_EACH(_GEN_DEL_ENT, __VA_ARGS__)

#define _GEN_LISTEN_FOR_LIKE_ENTITIES_INTERNALS(comp, i) \
  if(likeness & ENUM_##comp) { \
    registerAddCallback_##comp(additionDelegate);\
    registerRemCallback_##comp(removalDelegate); \
  }
#define GEN_LISTEN_FOR_LIKE_ENTITIES_INTERNALS(...) DO_FOR_EACH(_GEN_LISTEN_FOR_LIKE_ENTITIES_INTERNALS, __VA_ARGS__)

#define _GEN_ARG_NAME_NUMERIC(placeholder, i) , arg##i
#define GEN_ARG_NAMES(...) DO_FOR_EACH(_GEN_ARG_NAME_NUMERIC, ##__VA_ARGS__)

#define _GEN_ARG_NAME_TYPED(type, i) , type arg##i
#define GEN_ARG_NAMES_TYPED(...) DO_FOR_EACH(_GEN_ARG_NAME_TYPED, ##__VA_ARGS__)

#define _COMP_COLL_DECL(comp, ...) \
        private: \
        KvMap<entityId, comp> comps_##comp; \
        std::vector<EntNotifyDelegate> addCallbacks_##comp; \
        std::vector<EntNotifyDelegate> remCallbacks_##comp; \
        public: \
        CompOpReturn add##comp(const entityId& id GEN_ARG_NAMES_TYPED(__VA_ARGS__)); \
        CompOpReturn rem##comp(const entityId& id); \
        CompOpReturn get##comp(const entityId& id, comp** out); \
        void registerAddCallback_##comp (EntNotifyDelegate& dlgt); \
        void registerRemCallback_##comp (EntNotifyDelegate& dlgt);

#define GEN_COMP_COLL_DECL(comp) _COMP_COLL_DECL(comp, SIG_##comp)

#define _COMP_COLL_DEFN(comp, ...) \
  CompOpReturn State::add##comp(const entityId& id GEN_ARG_NAMES_TYPED(__VA_ARGS__)) \
                              { return addComp(comps_##comp, id, addCallbacks_##comp GEN_ARG_NAMES(__VA_ARGS__)); }\
  CompOpReturn State::rem##comp(const entityId& id) { return remComp(comps_##comp, id, remCallbacks_##comp); }\
  CompOpReturn State::get##comp(const entityId& id, comp** out) { return getComp(comps_##comp, id, out); } \
  void State::registerAddCallback_##comp (EntNotifyDelegate& dlgt) { addCallbacks_##comp.push_back(dlgt); } \
  void State::registerRemCallback_##comp (EntNotifyDelegate& dlgt) { remCallbacks_##comp.push_back(dlgt); }
#define GEN_COMP_COLL_DEFN(comp) _COMP_COLL_DEFN(comp, SIG_##comp)

#define GEN_COMP_DEFN_REQD(comp, flags) template<> compMask Component<comp>::requiredComps = flags; \
                                    template<> compMask Component<comp>::flag = ENUM_##comp
#define GEN_COMP_DEFN_DEPN(comp, flags) template<> compMask Component<comp>::dependentComps = flags

#endif //ECS_AUTOGEN_H
