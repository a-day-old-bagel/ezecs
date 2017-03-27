//
// Created by Galen on 2017-03-24.
//

#include <sstream>
#include <fstream>
#include <regex>
#include <iostream>
#include <vector>
#include <unordered_map>

#define TAB "  "

using namespace std;

/*
 * This holds everything we need to know about a component type in order to generate all the associated code.
 */
struct CompType {
  string name = "";
  vector<string> prerequisiteComps;
  vector<string> dependentComps;
  vector<string> constructorArgs;
};

/*
 * This holds information gathered from the users calls to the EZECS_COMPONENT_DEPENDENCIES macro
 */
struct CompDepEntry {
  string dependent = "";
  vector<string> prerequisites;
  string code;
};

/*
 * This is called by CMake
 */
int main(int argc, char *argv[]) {
  // CMake should provide exactly 3 arguments after the command name (4 total).
  if (argc < 4) { return 1; }

  // set up all file names for reading and writing
  string sourceDir = argv[1];
  string binDir = argv[2];
  string fileName_configIn = argv[3];
  string fileName_compsHIn = sourceDir + "/ecsComponents.hpp";
  string fileName_compsCIn = sourceDir + "/ecsComponents.cpp";
  string fileName_compsHOut = binDir + "/ecsComponents.generated.hpp";
  string fileName_compsCOut = binDir + "/ecsComponents.generated.cpp";
  string fileName_stateHIn = sourceDir + "/ecsState.hpp";
  string fileName_stateCIn = sourceDir + "/ecsState.cpp";
  string fileName_stateHOut = binDir + "/ecsState.generated.hpp";
  string fileName_stateCOut = binDir + "/ecsState.generated.cpp";

  // read in and store all input files (next five sections of code)
  ifstream configIn(fileName_configIn);
  if (!configIn) { return 1; }
  stringstream ss_configIn;
  ss_configIn << configIn.rdbuf();
  string str_configIn(ss_configIn.str());
  configIn.close();

  ifstream compsHIn(fileName_compsHIn);
  if (!compsHIn) { return 1; }
  stringstream ss_compsHIn;
  ss_compsHIn << compsHIn.rdbuf();
  string str_compsHIn(ss_compsHIn.str());
  compsHIn.close();

  ifstream compsCIn(fileName_compsCIn);
  if (!compsCIn) { return 1; }
  stringstream ss_compsCIn;
  ss_compsCIn << compsCIn.rdbuf();
  string str_compsCIn(ss_compsCIn.str());
  compsCIn.close();

  ifstream stateHIn(fileName_stateHIn);
  if (!stateHIn) { return 1; }
  stringstream ss_stateHIn;
  ss_stateHIn << stateHIn.rdbuf();
  string str_stateHIn(ss_stateHIn.str());
  stateHIn.close();

  ifstream stateCIn(fileName_stateCIn);
  if (!stateCIn) { return 1; }
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
  regex rx_confCodeAll("\\/\\/\\s*BEGIN\\s*DECLARATIONS\\s*((?:.|\\s*)*)");
  const char *confIn = str_configIn.c_str();
  auto rxit = cregex_iterator(confIn, confIn + strlen(confIn), rx_confCodeAll);
  if (rxit != cregex_iterator()) {
    cmatch match = *rxit;
    code_confAll = match[1].str();
  } else {
    cerr << "Parsing provided ezecs config file: Could not identify comment '// BEGIN DECLARATIONS' :"
         << " Make sure that comment exists and is formatted and placed correctly." << endl;
    return 1;
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
    return 1;
  }

  // Get just the definitions section of the config file code
  string code_confDefnsDirty;
  regex rx_confCodeBegDefn("\\/\\/\\s*BEGIN\\s*DEFINITIONS\\s*");
  rxit = cregex_iterator(confDecls, confDecls + strlen(confDecls), rx_confCodeBegDefn);
  if (rxit != cregex_iterator()) {
    cmatch match = *rxit;
    code_confDefnsDirty = code_confAll.substr((size_t)match.position() + match.length(), code_confAll.length());
  } else {
    cerr << "Parsing provided ezecs config file: Could not identify comment '// BEGIN DEFINITIONS' :"
         << " Make sure that comment exists and is formatted and placed correctly." << endl;
    return 1;
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
    return 1;
  }

  /*
   * compTypes will have keys that are component type names and values that are CompType (component type descriptions)
   */
  unordered_map<string, CompType> compTypes;

  // Fill compTypes with the user's component types from the config file (just find the name of each one)
  regex rx_confCompDecls("(?:class|struct)\\s*(\\w*)\\s*:\\s*public\\s*Component\\s*<\\s*\\w*\\s*>");
  for (auto it = cregex_iterator(confIn, confIn + strlen(confIn), rx_confCompDecls); it != cregex_iterator(); ++it) {
    cmatch match = *it;
    compTypes[match[1].str()] = { match[1].str() };
  }

  // Fill compType's 'prerequisiteComps' fields given the user's calls to the EZECS_COMPONENT_DEPENDENCIES macro
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
          return 1;
        }
      } else {
        if (compTypes.count(token)) {
          compType->prerequisiteComps.push_back(token);
        } else {
          cerr << "Invalid use of EZECS_COMPONENT_DEPENDENCIES (first arg: '" << compType->name
               << "'. invalid arg given: '" << token << "'.)" << endl;
          return 1;
        }
      }
    }
  }

  // Fill compType's 'dependentComps' fields given the now-filled 'prerequisiteComps' fields
  for (auto compType : compTypes) {
    for (auto preq : compType.second.prerequisiteComps) {
      compTypes.at(preq).dependentComps.push_back(compType.second.name);
    }
  }



  stringstream ss_compTypeList;
  for (auto type : compTypes) {
    cout << type.first << " (";
    for (auto preq : type.second.prerequisiteComps) {
      cout << preq << (preq == type.second.prerequisiteComps.back() ? "" : ", ");
    }
    cout << ") (";
    for (auto depn : type.second.dependentComps) {
      cout << depn << (depn == type.second.dependentComps.back() ? "" : ", ");
    }
    cout << ")" << endl;
    ss_compTypeList << TAB "// " << type.first << endl;
  }
  string str_compTypeList = ss_compTypeList.str();
  cout << endl;





  // replace "appears here" comments in the input file strings with code, write output file strings (next four sections)
  regex rx_compDecls("  \\/\\/ COMPONENT DECLARATIONS APPEAR HERE");
  regex rx_compEnums("    \\/\\/ COMPONENT TYPE ENUMERATORS APPEAR HERE");
  regex rx_numCompTypes("  \\/\\/ NUMBER OF COMPONENT TYPES APPEARS HERE");
  string str_compsHOut = regex_replace(str_compsHIn, rx_compDecls, TAB + code_confDecls);
  str_compsHOut = regex_replace(str_compsHOut, rx_compEnums, str_compTypeList);
  str_compsHOut = regex_replace(str_compsHOut, rx_numCompTypes, str_compTypeList);

  regex rx_compDepDef("  \\/\\/ COMPONENT DEPENDENCY FIELD DEFINITIONS APPEAR HERE");
  regex rx_compMetDef("  \\/\\/ COMPONENT METHOD DEFINITIONS APPEAR HERE");
  regex rx_compDepMutDef("  \\/\\/ COMPONENT DEPENDENCY FIELD GETTER DEFINITIONS APPEAR HERE");
  string str_compsCOut = regex_replace(str_compsCIn, rx_compDepDef, str_compTypeList);
  str_compsCOut = regex_replace(str_compsCOut, rx_compMetDef, TAB + code_confDefns);
  str_compsCOut = regex_replace(str_compsCOut, rx_compDepMutDef, str_compTypeList);

  regex rx_compCollDecls("      \\/\\/ COMPONENT COLLECTION AND MANIPULATION METHOD DECLARATIONS APPEAR HERE");
  string str_stateHOut = regex_replace(str_stateHIn, rx_compCollDecls, str_compTypeList);

  regex rx_compClrLoop("    \\/\\/ A LOOP TO CLEAR ALL COMPONENTS APPEARS HERE");
  regex rx_compDelLoop("    \\/\\/ A LOOP TO DELETE ALL COMPONENTS APPEARS HERE");
  regex rx_compRegCllbks("    \\/\\/ CODE TO REGISTER THE APPROPRIATE CALLBACKS APPEARS HERE");
  regex rx_compCollDef("  \\/\\/ COMPONENT COLLECTION MANIPULATION METHOD DEFINITIONS APPEAR HERE");
  string str_stateCOut = regex_replace(str_stateCIn, rx_compClrLoop, str_compTypeList);
  str_stateCOut = regex_replace(str_stateCOut, rx_compDelLoop, str_compTypeList);
  str_stateCOut = regex_replace(str_stateCOut, rx_compRegCllbks, str_compTypeList);
  str_stateCOut = regex_replace(str_stateCOut, rx_compCollDef, str_compTypeList);

  // write the output file strings to the appropriate files (next four sections of code)
  ofstream compsHOut(fileName_compsHOut);
  if (!compsHOut) { return 1; }
  compsHOut << "/*\n * EZECS - The E-Z Entity Component System\n * Header generated using " << argv[1] << "\n */\n\n";
  compsHOut << str_compsHOut;
  compsHOut.close();

  ofstream compsCOut(fileName_compsCOut);
  if (!compsCOut) { return 1; }
  compsCOut << "/*\n * EZECS - The E-Z Entity Component System\n * Source generated using " << argv[1] << "\n */\n\n";
  compsCOut << str_compsCOut;
  compsCOut.close();

  ofstream stateHOut(fileName_stateHOut);
  if (!stateHOut) { return 1; }
  stateHOut << "/*\n * EZECS - The E-Z Entity Component System\n * Header generated using " << argv[1] << "\n */\n\n";
  stateHOut << str_stateHOut;
  stateHOut.close();

  ofstream stateCOut(fileName_stateCOut);
  if (!stateCOut) { return 1; }
  stateCOut << "/*\n * EZECS - The E-Z Entity Component System\n * Source generated using " << argv[1] << "\n */\n\n";
  stateCOut << str_stateCOut;
  stateCOut.close();


  // Make some test files (temporary - erase later) TODO: erase this
  string fileName_testHOut = binDir + "/test.generated.hpp";
  ofstream headerOut(fileName_testHOut);
  if (!headerOut) { return 1; }
  headerOut << "/*\n * EZECS - The E-Z Entity Component System\n * Header generated using "
            << argv[1] << "\n */\n\n"
            << "#include <iostream>\n"
            << "namespace ezecs {\n"
            << TAB "void testFunction() {\n"
            << TAB TAB "printf(\"generated code works again!\\n\");\n"
            << TAB "}\n"
            << "}\n";
  headerOut.close();

  string fileName_testCOut = binDir + "/test.generated.cpp";
  ofstream sourceOut(fileName_testCOut);
  if (!sourceOut) { return 1; }
  sourceOut << "/*\n * EZECS - The E-Z Entity Component System\n * Source generated using "
            << argv[1] << "\n */\n";
  sourceOut.close();

  return 0;
}

#undef TAB
