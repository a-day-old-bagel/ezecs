/*
 * Automatic code generator for ezecs. This code is not included in any of the
 * libraries produced when you build ezecs. It just generates some source code
 * when you provide CMake with the variable EZECS_CONFIG_FILE, which should
 * point to a component config file that you wrote. Using this generator in an
 * environment other than CMake's is not supported, but you probably could if
 * you really really wanted to.
 */
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

#include <sstream>
#include <fstream>
#include <regex>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>

using namespace std;

#define TAB "  "
const string pathSeperator =
#ifdef _WIN32
"\\";
#else
"/";
#endif

// Prototype helper methods
string enumStringIzer(const string& compType);
string genStateHPrivatSection(const string &compType);
string genStateHPublicSection(const string &compType, const string &compArgs);
string genStateCDefns(const string &compType, const string &compArgs, const string &compArgsNameOnly);
string getNamesFromArgList(string argList);
string replaceAndCount(const string& inStr, const regex& rx, const string& reStr, unsigned long& count);
unsigned long occurrences(const string& s, const char c);
/*
 * CompType holds everything we need to know about a component type in order to generate all the associated code.
 */
struct CompType {
  string name = "";
  string constructorArgs, constructorArgsNamesOnly, enumName, stateH_prv, stateH_pub, stateC;
  vector<string> prerequisiteComps;
  vector<string> dependentComps;

  void resolveSimpleStrings() {
    enumName = enumStringIzer(name);
    stateH_prv = genStateHPrivatSection(name);
    stateH_pub = genStateHPublicSection(name, constructorArgs);
    constructorArgsNamesOnly = getNamesFromArgList(constructorArgs);
    stateC = genStateCDefns(name, constructorArgs, constructorArgsNamesOnly);
  }
};

/*
 * main is called by CMake
 * It takes an input directory, an output directory, and a configuration file.
 */
int main(int argc, char *argv[]) {
  // CMake should provide exactly 3 arguments after the command name (4 total).
  if (argc < 4) {
    cerr << "ecsGenerator: Did not receive correct number of arguments." << endl;
    return -1;
  }

  // set up all file names for reading and writing
  string sourceDir = argv[1];
  string binDir = argv[2];
  string fileName_configIn = argv[3];
  string fileName_compsHIn = sourceDir + pathSeperator + "ecsComponents.hpp";
  string fileName_compsCIn = sourceDir + pathSeperator + "ecsComponents.cpp";
  string fileName_compsHOut = binDir + pathSeperator + "ecsComponents.generated.hpp";
  string fileName_compsCOut = binDir + pathSeperator + "ecsComponents.generated.cpp";
  string fileName_stateHIn = sourceDir + pathSeperator + "ecsState.hpp";
  string fileName_stateCIn = sourceDir + pathSeperator + "ecsState.cpp";
  string fileName_stateHOut = binDir + pathSeperator + "ecsState.generated.hpp";
  string fileName_stateCOut = binDir + pathSeperator + "ecsState.generated.cpp";

  // read in and store all input files (next five sections of code)
  ifstream configIn(fileName_configIn);
  if (!configIn) { return -2; }
  stringstream ss_configIn;
  ss_configIn << configIn.rdbuf();
  string str_configIn(ss_configIn.str());
  configIn.close();

  ifstream compsHIn(fileName_compsHIn);
  if (!compsHIn) { return -3; }
  stringstream ss_compsHIn;
  ss_compsHIn << compsHIn.rdbuf();
  string str_compsHIn(ss_compsHIn.str());
  compsHIn.close();

  ifstream compsCIn(fileName_compsCIn);
  if (!compsCIn) { return -4; }
  stringstream ss_compsCIn;
  ss_compsCIn << compsCIn.rdbuf();
  string str_compsCIn(ss_compsCIn.str());
  compsCIn.close();

  ifstream stateHIn(fileName_stateHIn);
  if (!stateHIn) { return -5; }
  stringstream ss_stateHIn;
  ss_stateHIn << stateHIn.rdbuf();
  string str_stateHIn(ss_stateHIn.str());
  stateHIn.close();

  ifstream stateCIn(fileName_stateCIn);
  if (!stateCIn) { return -6; }
  stringstream ss_stateCIn;
  ss_stateCIn << stateCIn.rdbuf();
  string str_stateCIn(ss_stateCIn.str());
  stateCIn.close();

  /*
   * code_includes will be filled with the include directives from the config file
   * code_confDecls will be filled with the component declarations from the config file
   * code_confDefns will be filled with the component definitions from the config file
   */
  string code_includes, code_confDecls, code_confDefns;

  // Get the entire block of code comprising the include directives
  regex rx_confCodeInclBegin("\\/\\/\\s*BEGIN\\s*INCLUDES\\s*");
  const char *confIn = str_configIn.c_str();
  auto rxit = cregex_iterator(confIn, confIn + strlen(confIn), rx_confCodeInclBegin);
  if (rxit != cregex_iterator()) {
    cmatch match = *rxit;
    code_includes = str_configIn.substr((size_t)match.position() + match.length(), str_configIn.length());
  } else {
    cerr << "Parsing provided ezecs config file: Could not identify comment '// BEGIN INCLUDES' :"
         << " Make sure that comment exists and is formatted and placed correctly." << endl;
    return -7;
  }

  // Get just the declarations section of the config file code
  regex rx_confCodeInclEnd("\\s*\\/\\/\\s*END\\s*INCLUDES");
  const char* confIncludes = code_includes.c_str();
  rxit = cregex_iterator(confIncludes, confIncludes + strlen(confIncludes), rx_confCodeInclEnd);
  if (rxit != cregex_iterator()) {
    cmatch match = *rxit;
    code_includes = code_includes.substr(0, (size_t)match.position());
  } else {
    cerr << "Parsing provided ezecs config file: Could not identify comment '// END INCLUDES' :"
         << " Make sure that comment exists and is formatted and placed correctly." << endl;
    return -8;
  }

  // Get the entire block of code comprising the component declarations and definitions from the config file
  string code_confAll;
  regex rx_confCodeAll("\\/\\/\\s*BEGIN\\s*DECLARATIONS\\s*");
  rxit = cregex_iterator(confIn, confIn + strlen(confIn), rx_confCodeAll);
  if (rxit != cregex_iterator()) {
    cmatch match = *rxit;
    code_confAll = str_configIn.substr((size_t)match.position() + match.length(), str_configIn.length());
  } else {
    cerr << "Parsing provided ezecs config file: Could not identify comment '// BEGIN DECLARATIONS' :"
         << " Make sure that comment exists and is formatted and placed correctly." << endl;
    return -7;
  }

  // Get just the declarations section of the config file code
  regex rx_confCodeEndDecl("\\s*\\/\\/\\s*END\\s*DECLARATIONS");
  const char* confDecls = code_confAll.c_str();
  rxit = cregex_iterator(confDecls, confDecls + strlen(confDecls), rx_confCodeEndDecl);
  if (rxit != cregex_iterator()) {
    cmatch match = *rxit;
    code_confDecls = code_confAll.substr(0, (size_t)match.position());
  } else {
    cerr << "Parsing provided ezecs config file: Could not identify comment '// END DECLARATIONS' :"
         << " Make sure that comment exists and is formatted and placed correctly." << endl;
    return -8;
  }

  // Get just the definitions section of the config file code (then cut off the trailing unrelated code)
  string code_confDefnsDirty;
  regex rx_confCodeBegDefn("\\/\\/\\s*BEGIN\\s*DEFINITIONS\\s*");
  rxit = cregex_iterator(confDecls, confDecls + strlen(confDecls), rx_confCodeBegDefn);
  if (rxit != cregex_iterator()) {
    cmatch match = *rxit;
    code_confDefnsDirty = code_confAll.substr((size_t)match.position() + match.length(), code_confAll.length());
  } else {
    cerr << "Parsing provided ezecs config file: Could not identify comment '// BEGIN DEFINITIONS' :"
         << " Make sure that comment exists and is formatted and placed correctly." << endl;
    return -9;
  }
  regex rx_confCodeEndDefn("\\s*\\/\\/\\s*END\\s*DEFINITIONS");
  const char* confDefnsDirty = code_confDefnsDirty.c_str();
  rxit = cregex_iterator(confDefnsDirty, confDefnsDirty + strlen(confDefnsDirty), rx_confCodeEndDefn);
  if (rxit != cregex_iterator()) {
    cmatch match = *rxit;
    code_confDefns = code_confDefnsDirty.substr(0, (size_t)match.position());
  } else {
    cerr << "Parsing provided ezecs config file: Could not identify comment '// END DEFINITIONS' :"
         << " Make sure that comment exists and is formatted and placed correctly." << endl;
    return -10;
  }
  
  /*
   * C-string versions of the above strings we just found
   */
  const char* decls = code_confDecls.c_str();
  const char* defns = code_confDefns.c_str();

  /*
   * compTypes will have keys that are component type names and values that are CompType (component type descriptions)
   * compTypeNames is just a vector of all of their names.
   */
  unordered_map<string, CompType> compTypes;
  vector<string> compTypeNames;

  // Fill compTypes' 'name' fields with the user's component type names from the config file.
  regex rx_confCompDecls("(?:class|struct)\\s*(\\w*)\\s*:\\s*public\\s*Component\\s*<\\s*\\w*\\s*>");
  for (auto it = cregex_iterator(decls, decls + strlen(decls), rx_confCompDecls); it != cregex_iterator(); ++it) {
    cmatch match = *it;
    CompType newType;
    newType.name = match[1].str();
    compTypes[match[1].str()] = newType;
    compTypeNames.push_back(match[1].str());
  }
  
  // Fill out compTypes' constructorArgs fields using those names, and then fill all the fields we can at this point.
  for (auto name : compTypeNames) {
    stringstream regexBuilder;
    regexBuilder << name << "\\s*::\\s*" << name;
    regexBuilder << "\\s*\\(\\s*(.*)\\s*\\)";
    regex rx_confCompCnstrctr(regexBuilder.str());
    auto it = cregex_iterator(defns, defns + strlen(defns), rx_confCompCnstrctr);
    if (it != cregex_iterator()) {
      cmatch match = *it;
      compTypes.at(name).constructorArgs = match[1].str();
      compTypes.at(name).resolveSimpleStrings();
    } else {
      cerr << "Could not find constructor definition for " << name <<"! Make sure there is an explicit definition "
           << "inside the DEFINITIONS secion of your config file." << endl;
      return -11;
    }
  }
  
  // Fill compTypes' 'prerequisiteComps' fields given the user's calls to the EZECS_COMPONENT_DEPENDENCIES macro
  regex rx_confCompDep("EZECS_COMPONENT_DEPENDENCIES\\s*\\((\\s*\\w*(?:\\s*,\\s*\\w+\\s*)*\\s*)\\)");
  for (auto it = cregex_iterator(confIn, confIn + strlen(confIn), rx_confCompDep); it != cregex_iterator(); ++it) {
    cmatch match = *it;
    stringstream ss_depArgs(match[1].str());
    CompType* compType = NULL;
    string token;
    bool first = true;
    while(std::getline(ss_depArgs, token, ',')) {
      token.erase( // remove whitespaces from string
          std::remove_if( token.begin(), token.end(), []( char ch ) {
            return std::isspace<char>( ch, std::locale::classic() );
          } ), token.end() );
      if (first) {
        first = false;
        try {
          compType = &compTypes.at(token);
        } catch (...) {
          cerr << "Invalid use of EZECS_COMPONENT_DEPENDENCIES (invalid first arg given: '" << token << "')" << endl;
          return -12;
        }
      } else {
        if (compTypes.count(token)) {
          compType->prerequisiteComps.push_back(token);
        } else {
          cerr << "Invalid use of EZECS_COMPONENT_DEPENDENCIES (first arg: '" << compType->name
               << "'. invalid arg given: '" << token << "'.)" << endl;
          return -13;
        }
      }
    }
  }

  // Fill compTypes' 'dependentComps' fields given the now-filled 'prerequisiteComps' fields
  for (auto name : compTypeNames) {
    for (auto preq : compTypes.at(name).prerequisiteComps) {
      compTypes.at(preq).dependentComps.push_back(name);
    }
  }
  
  // Build the string that goes in the component enumerators spot
  stringstream ss_code_compEnum;
  int i = 0;
  for (auto name : compTypeNames) {
    ss_code_compEnum << TAB TAB << compTypes.at(name).enumName << " = 1 << " << ++i << "," << endl;
  }
  string code_compEnum = ss_code_compEnum.str();
  
  // Build the string that declares the number of user-made components
  stringstream ss_code_numComps;
  ss_code_numComps << TAB "const uint8_t numCompTypes = " << i << ";" << endl;
  string code_numComps = ss_code_numComps.str();
  
  // Build the string that defines the component dependency relationships
  stringstream ss_code_compDepends;
  for (auto name : compTypeNames) {
    // required comps
    string requiredComps;
    stringstream ss_requiredComps;
    ss_requiredComps << enumStringIzer("Existence");
    for (auto comp : compTypes.at(name).prerequisiteComps) {
      ss_requiredComps << " | " << compTypes.at(comp).enumName;
    }
    requiredComps = ss_requiredComps.str();
    ss_code_compDepends << endl << TAB "template<> compMask Component<" << name << ">::requiredComps = "
                        << requiredComps << ";" << endl;

    // dependent comps
    string dependentComps;
    if (compTypes.at(name).dependentComps.empty()) {
      dependentComps = "NONE";
    } else {
      stringstream ss_dependentComps;
      for (auto comp : compTypes.at(name).dependentComps) {
        ss_dependentComps << compTypes.at(comp).enumName
                          << (comp == compTypes.at(name).dependentComps.back() ? "" : " | ");
      }
      dependentComps = ss_dependentComps.str();
    }
    ss_code_compDepends << TAB "template<> compMask Component<" << name << ">::dependentComps = "
                        << dependentComps << ";" << endl;

    // self flag
    ss_code_compDepends << TAB "template<> compMask Component<" << name << ">::flag = "
                        << compTypes.at(name).enumName << ";" << endl;
  }
  string code_compDepends = ss_code_compDepends.str();
  
  // Build the strings that are used in the component dependency getter switch-case statements
  stringstream ss_code_compGetReq;
  stringstream ss_code_compGetDep;
  for (auto name : compTypeNames) {
    ss_code_compGetReq << TAB TAB TAB "case " << compTypes.at(name).enumName << ": return "
                       << name << "::requiredComps;" << endl;
    ss_code_compGetDep << TAB TAB TAB "case " << compTypes.at(name).enumName << ": return "
                       << name << "::dependentComps;" << endl;
  }
  string code_compGetReq = ss_code_compGetReq.str();
  string code_compGetDep = ss_code_compGetDep.str();
  
  // Build the string that declares collections, methods, and stuff in ecsState.generated.hpp
  stringstream ss_code_stateHOut;
  for (auto name : compTypeNames) {
    ss_code_stateHOut << compTypes.at(name).stateH_prv << endl;
  }
  ss_code_stateHOut << endl << TAB TAB << "public:" << endl;
  for (auto name : compTypeNames) {
    ss_code_stateHOut << endl << compTypes.at(name).stateH_pub;
  }
  string code_stateHOut = ss_code_stateHOut.str();

  // Build a string for the stuff in the 'clear all components' loop
  stringstream ss_code_clearCompLoop;
  for (auto name : compTypeNames) {
    string enm = compTypes.at(name).enumName;
    ss_code_clearCompLoop
        << TAB TAB "remCompNoChecks(comps_" << name << ", existence, id, remCallbacks_" << name << ");" << endl;
  }
  string code_clearCompLoop = ss_code_clearCompLoop.str();

  // Build a string for the entity likeness callback registration
  stringstream ss_code_cllbkReg;
  for (auto name : compTypeNames) {
    ss_code_cllbkReg << TAB TAB "if (likeness & " << compTypes.at(name).enumName << ") {" << endl;
    ss_code_cllbkReg << TAB TAB TAB "registerAddCallback_" << name << "(additionDelegate);" << endl;
    ss_code_cllbkReg << TAB TAB TAB "registerRemCallback_" << name << "(removalDelegate);" << endl;
    ss_code_cllbkReg << TAB TAB "}" << endl;
  }
  string code_cllbkReg = ss_code_cllbkReg.str();

  // Build a string for the collection manipulation methods
  stringstream ss_code_compCollDefns;
  for (auto name : compTypeNames) {
    ss_code_compCollDefns << compTypes.at(name).stateC << endl;
  }
  string code_compCollDefns = ss_code_compCollDefns.str();
  
  // keep count of lines generated
  unsigned long lineCount = 0;

  // replace "appears here" comments in the input file strings with code (next four sections)
  regex rx_compIncls("\\/\\/ EXTRA INCLUDES APPEAR HERE");
  regex rx_compDecls("  \\/\\/ COMPONENT DECLARATIONS APPEAR HERE");
  regex rx_compEnums("    \\/\\/ COMPONENT TYPE ENUMERATORS APPEAR HERE");
  regex rx_numCompTypes("  \\/\\/ NUMBER OF COMPONENT TYPES APPEARS HERE");
  string str_compsHOut = replaceAndCount(str_compsHIn, rx_compIncls, code_includes, lineCount);
  str_compsHOut = replaceAndCount(str_compsHOut, rx_compDecls, TAB + code_confDecls, lineCount);
  str_compsHOut = replaceAndCount(str_compsHOut, rx_compEnums, code_compEnum, lineCount);
  str_compsHOut = replaceAndCount(str_compsHOut, rx_numCompTypes, code_numComps, lineCount);

  regex rx_compDepDef("  \\/\\/ COMPONENT DEPENDENCY FIELD DEFINITIONS APPEAR HERE");
  regex rx_compMetDef("  \\/\\/ COMPONENT METHOD DEFINITIONS APPEAR HERE");
  regex rx_compGetReqDef("      \\/\\/ COMPONENT REQUIREMENTS GETTER CASES APPEAR HERE");
  regex rx_compGetDepDef("      \\/\\/ COMPONENT DEPENDENTS GETTER CASES APPEAR HERE");
  string str_compsCOut = replaceAndCount(str_compsCIn, rx_compDepDef, code_compDepends, lineCount);
  str_compsCOut = replaceAndCount(str_compsCOut, rx_compMetDef, TAB + code_confDefns, lineCount);
  str_compsCOut = replaceAndCount(str_compsCOut, rx_compGetReqDef, code_compGetReq, lineCount);
  str_compsCOut = replaceAndCount(str_compsCOut, rx_compGetDepDef, code_compGetDep, lineCount);

  regex rx_compCollDecls("      \\/\\/ COMPONENT COLLECTION AND MANIPULATION METHOD DECLARATIONS APPEAR HERE");
  string str_stateHOut = replaceAndCount(str_stateHIn, rx_compCollDecls, code_stateHOut, lineCount);

  regex rx_compClrLoop("    \\/\\/ A LOOP TO CLEAR ALL COMPONENTS APPEARS HERE");
  regex rx_compRegCllbks("    \\/\\/ CODE TO REGISTER THE APPROPRIATE CALLBACKS APPEARS HERE");
  regex rx_compCollDef("  \\/\\/ COMPONENT COLLECTION MANIPULATION METHOD DEFINITIONS APPEAR HERE");
  string str_stateCOut = replaceAndCount(str_stateCIn, rx_compClrLoop, code_clearCompLoop, lineCount);
  str_stateCOut = replaceAndCount(str_stateCOut, rx_compRegCllbks, code_cllbkReg, lineCount);
  str_stateCOut = replaceAndCount(str_stateCOut, rx_compCollDef, code_compCollDefns, lineCount);

  // make some file header intro text for header and source files (next two sections)
  stringstream ss_hIntro;
  ss_hIntro << "/*\n * EZECS - The E-Z Entity Component System\n * Header generated using " << argv[3] << "\n */\n\n";
  string hppIntro = ss_hIntro.str();
  
  stringstream ss_cIntro;
  ss_cIntro << "/*\n * EZECS - The E-Z Entity Component System\n * Source generated using " << argv[3] << "\n */\n\n";
  string cppIntro = ss_cIntro.str();
  
  // write the output file strings to the appropriate files (next four sections of code)
  ofstream compsHOut(fileName_compsHOut);
  if (!compsHOut) { return -14; }
  compsHOut << hppIntro << str_compsHOut;
  compsHOut.close();

  ofstream compsCOut(fileName_compsCOut);
  if (!compsCOut) { return -15; }
  compsCOut << cppIntro << str_compsCOut;
  compsCOut.close();

  ofstream stateHOut(fileName_stateHOut);
  if (!stateHOut) { return -16; }
  stateHOut << hppIntro << str_stateHOut;
  stateHOut.close();

  ofstream stateCOut(fileName_stateCOut);
  if (!stateCOut) { return -17; }
  stateCOut << cppIntro << str_stateCOut;
  stateCOut.close();

  // Give some feedback
  for (auto name : compTypeNames) {
    stringstream colName, colReq, colDep, colCon;
    colName << left << name << " [" << compTypes.at(name).enumName << "]";
    colReq << "REQ(";
    for (auto preq : compTypes.at(name).prerequisiteComps) {
      colReq << preq << (preq == compTypes.at(name).prerequisiteComps.back() ? "" : ", ");
    }
    colReq << ")";
    colDep << "DEP(";
    for (auto depn : compTypes.at(name).dependentComps) {
      colDep << depn << (depn == compTypes.at(name).dependentComps.back() ? "" : ", ");
    }
    colDep << ")";
    colCon << "CON(" << compTypes.at(name).constructorArgs << ")";
    cout << setw(32) << left << colName.str();
    cout << setw(32) << left << colReq.str();
    cout << setw(32) << left << colDep.str();
    cout << setw(32) << left << colCon.str() << endl;
  }
  // report lines generated
  cout << "LINES OF CODE GENERATED: " << lineCount << endl;

  return 0;
}

/*
 * This converts a given component type name into an enumerator name.
 */
string enumStringIzer(const string& compType) {
  string result = compType;
  transform(result.begin(), result.end(), result.begin(), ::toupper);
  return result;
}

/*
 * This converts a given component type name into the private portions of the declarations of that component's
 * collection, collection manipulator methods, and callback structures.
 */
string genStateHPrivatSection(const string &compType) {
  stringstream result;
  result << TAB TAB TAB "KvMap<entityId, " << compType << "> comps_" << compType << ";" << endl;
  result << TAB TAB TAB "std::vector<EntNotifyDelegate> addCallbacks_" << compType << ";" << endl;
  result << TAB TAB TAB "std::vector<EntNotifyDelegate> remCallbacks_" << compType << ";" << endl;
  return result.str();
}

/*
 * This converts a given component type name and constructor arguments into the public portions of the declarations of
 * that component's collection, collection manipulator methods, and callback registration methods.
 */
string genStateHPublicSection(const string &compType, const string &compArgs) {
  stringstream result;
  result << TAB TAB TAB "CompOpReturn add_" << compType << "(const entityId& id, " << compArgs << ");" << endl;
  result << TAB TAB TAB "CompOpReturn rem_" << compType << "(const entityId& id);" << endl;
  result << TAB TAB TAB "CompOpReturn get_" << compType << "(const entityId& id, " << compType << "** out);" << endl;
  result << TAB TAB TAB "void registerAddCallback_" << compType << "(EntNotifyDelegate& dlgt);" << endl;
  result << TAB TAB TAB "void registerRemCallback_" << compType << "(EntNotifyDelegate& dlgt);" << endl;
  return result.str();
}

/*
 * This converts a given component type name and constructor arguments into the definitions of
 * that component's collection, collection manipulator methods, and callback registration methods.
 */
string genStateCDefns(const string &compType, const string &compArgs, const string &compArgsNameOnly) {
  stringstream result;
  result << TAB "CompOpReturn State::add_" << compType << "(const entityId& id, " << compArgs << ") {" << endl;
  result << TAB TAB "return addComp(comps_" << compType << ", id, addCallbacks_" << compType << ", "
         << compArgsNameOnly << ");" << endl;
  result << TAB "}" << endl;
  result << TAB "CompOpReturn State::rem_" << compType << "(const entityId& id) {" << endl;
  result << TAB TAB "return remComp(comps_" << compType << ", id, remCallbacks_" << compType << ");" << endl;
  result << TAB "}" << endl;
  result << TAB "CompOpReturn State::get_" << compType << "(const entityId& id, " << compType << "** out) {" << endl;
  result << TAB TAB "return getComp(comps_" << compType << ", id, out);" << endl;
  result << TAB "}" << endl;
  result << TAB "void State::registerAddCallback_" << compType << "(EntNotifyDelegate& dlgt) {" << endl;
  result << TAB TAB "addCallbacks_" << compType << ".push_back(dlgt);" << endl;
  result << TAB "}" << endl;
  result << TAB "void State::registerRemCallback_" << compType << "(EntNotifyDelegate& dlgt) {" << endl;
  result << TAB TAB "remCallbacks_" << compType << ".push_back(dlgt);" << endl;
  result << TAB "}" << endl;
  return result.str();
}

/*
 * Given a string formatted as a typed argument list (EX. "type0 name0, type1 name1, type2 name2, ..."),
 * this extracts just the names and puts them in a non-typed list (EX. "name0, name1, name2, ...").
 */
string getNamesFromArgList(string argList) {
  replace(argList.begin(), argList.end(), ',' , ' ');
  stringstream input(argList), output;
  string singleTerm;
  bool oddPicker = false, first = true;
  while (input >> singleTerm) {
    if (oddPicker) {
      if (first) { first = false; }
      else { output << ", "; }
      output << singleTerm;
    }
    oddPicker = !oddPicker;
  }
  return output.str();
}

/*
 * A helper to keep track of how many lines of code have been inserted using regex_replace
 */
string replaceAndCount(const string& inStr, const regex& rx, const string& reStr, unsigned long& count) {
  count += occurrences(reStr, '\n');
  return regex_replace(inStr, rx, reStr);
}


/*
 * Just a helper function to count character occurrences in a string
 */
unsigned long occurrences(const string& s, const char c) {
  unsigned long i = 0, count = 0, contain;
  while((contain = s.find(c,i)) != string::npos){
    count++;
    i = contain + 1;
  }
  return count;
}

#undef TAB
