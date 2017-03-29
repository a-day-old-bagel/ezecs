//
// Created by Galen on 2017-03-24.
//

#include <sstream>
#include <fstream>
#include <regex>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

#define TAB "  "
const string pathSeperator =
#ifdef _WIN32
"\\";
#else
"/";
#endif

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
string collStringIzer_private(const string& compType) {
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
string collStringIzer_public(const string& compType, const string& compArgs) {
  stringstream result;
  result << TAB TAB TAB "CompOpReturn add" << compType << "(const entityId& id, " << compArgs << ");" << endl;
  result << TAB TAB TAB "CompOpReturn rem" << compType << "(const entityId& id);" << endl;
  result << TAB TAB TAB "CompOpReturn get" << compType << "(const entityId& id, " << compType << "** out);" << endl;
  result << TAB TAB TAB "void registerAddCallback_" << compType << "(EntNotifyDelegate& dlgt);" << endl;
  result << TAB TAB TAB "void registerRemCallback_" << compType << "(EntNotifyDelegate& dlgt);" << endl;
  return result.str();
}

/*
 * This holds everything we need to know about a component type in order to generate all the associated code.
 */
struct CompType {
  string name = "";
  string constructorArgs, enumName, stateH_prv, stateH_pub;
  vector<string> prerequisiteComps;
  vector<string> dependentComps;
  
  void resolveNameBasedStrings() {
    enumName = enumStringIzer(name);
    stateH_prv = collStringIzer_private(name);
    stateH_pub = collStringIzer_public(name, constructorArgs);
  }
};

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

/*
 * A helper to keep track of how many lines of code have been inserted using regex_replace
 */
string replaceAndCount(const string& inStr, const regex& rx, const string& reStr, unsigned long& count) {
  count += occurrences(reStr, '\n');
  return regex_replace(inStr, rx, reStr);
}

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
   * code_confDecls will be filled with the component declarations from the config file
   * code_confDefns will be filled with the component definitions from the config file
   */
  string code_confDecls, code_confDefns;

  // Get the entire block of code comprising the component declarations and definitions from the config file
  string code_confAll;
  regex rx_confCodeAll("\\/\\/\\s*BEGIN\\s*DECLARATIONS\\s*");
  const char *confIn = str_configIn.c_str();
  auto rxit = cregex_iterator(confIn, confIn + strlen(confIn), rx_confCodeAll);
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
    compTypes[match[1].str()] = { match[1].str() };
    compTypeNames.push_back(match[1].str());
  }
  
  // Fill out compTypes' constructorArgs fields using those names, and then fill all the fields we can at this point.
  for (auto name : compTypeNames) {
    stringstream regexBuilder;
    regexBuilder << name << "\\s*::\\s*" << name;
    regexBuilder << "\\s*\\(\\s*((?:\\w+\\s+\\w+(?:\\s*,\\s*\\w+\\s+\\w+)*)?)\\s*\\)";
    regex rx_confCompCnstrctr(regexBuilder.str());
    auto it = cregex_iterator(defns, defns + strlen(defns), rx_confCompCnstrctr);
    if (it != cregex_iterator()) {
      cmatch match = *it;
      compTypes.at(name).constructorArgs = match[1].str();
      compTypes.at(name).resolveNameBasedStrings();
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



  // Debug components TODO: remove this section
  stringstream ss_compTypeList;
  for (auto name : compTypeNames) {
    cout << name << " (";
    for (auto preq : compTypes.at(name).prerequisiteComps) {
      cout << preq << (preq == compTypes.at(name).prerequisiteComps.back() ? "" : ", ");
    }
    cout << ") (";
    for (auto depn : compTypes.at(name).dependentComps) {
      cout << depn << (depn == compTypes.at(name).dependentComps.back() ? "" : ", ");
    }
    cout << ") | " << compTypes.at(name).enumName << " | (" << compTypes.at(name).constructorArgs << ")" << endl;
    ss_compTypeList << TAB "// " << name << endl;
  }
  string str_compTypeList = ss_compTypeList.str();
  cout << endl;

  
  
  
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
  ss_code_stateHOut << TAB TAB << endl << "public:" << endl;
  for (auto name : compTypeNames) {
    ss_code_stateHOut << endl << compTypes.at(name).stateH_pub;
  }
  string code_stateHOut = ss_code_stateHOut.str();
  

  
  // keep count of lines generated
  unsigned long lineCount = 0;

  // replace "appears here" comments in the input file strings with code (next four sections)
  regex rx_compDecls("  \\/\\/ COMPONENT DECLARATIONS APPEAR HERE");
  regex rx_compEnums("    \\/\\/ COMPONENT TYPE ENUMERATORS APPEAR HERE");
  regex rx_numCompTypes("  \\/\\/ NUMBER OF COMPONENT TYPES APPEARS HERE");
  string str_compsHOut = replaceAndCount(str_compsHIn, rx_compDecls, TAB + code_confDecls, lineCount);
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
  regex rx_compDelLoop("    \\/\\/ A LOOP TO DELETE ALL COMPONENTS APPEARS HERE");
  regex rx_compRegCllbks("    \\/\\/ CODE TO REGISTER THE APPROPRIATE CALLBACKS APPEARS HERE");
  regex rx_compCollDef("  \\/\\/ COMPONENT COLLECTION MANIPULATION METHOD DEFINITIONS APPEAR HERE");
  string str_stateCOut = replaceAndCount(str_stateCIn, rx_compClrLoop, str_compTypeList, lineCount);
  str_stateCOut = replaceAndCount(str_stateCOut, rx_compDelLoop, str_compTypeList, lineCount);
  str_stateCOut = replaceAndCount(str_stateCOut, rx_compRegCllbks, str_compTypeList, lineCount);
  str_stateCOut = replaceAndCount(str_stateCOut, rx_compCollDef, str_compTypeList, lineCount);
  
  // report lines generated
  cout << "LINES OF CODE GENERATED: " << lineCount << endl;

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


  // Make some test files (temporary - erase later) TODO: erase this (next two sections)
  string fileName_testHOut = binDir + "/test.generated.hpp";
  ofstream headerOut(fileName_testHOut);
  if (!headerOut) { return -18; }
  headerOut << hppIntro
            << "#include <iostream>\n"
            << "namespace ezecs {\n"
            << TAB "void testFunction() {\n"
            << TAB TAB "printf(\"generated code works again!\\n\");\n"
            << TAB "}\n"
            << "}\n";
  headerOut.close();

  string fileName_testCOut = binDir + "/test.generated.cpp";
  ofstream sourceOut(fileName_testCOut);
  if (!sourceOut) { return -19; }
  sourceOut << cppIntro;
  sourceOut.close();

  return 0;
}

#undef TAB
