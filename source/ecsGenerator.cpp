//
// Created by Galen on 2017-03-24.
//

#include <sstream>
#include <fstream>
#include <regex>
#include <iostream>
#include <vector>

#define TAB "    "

using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::regex;
using std::string;
using std::regex_replace;
using std::cregex_iterator;
using std::strlen;
using std::cmatch;
using std::vector;

struct compType {
  string name;
  vector<string> constructorArgs;
};

int main(int argc, char* argv[]) {
  // arguments provided by CMake
  if (argc < 3) { return 1; }
  
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
  
  vector<compType> compTypes;
  regex rx_compNameFinder("(?:class|struct)\\s*(\\w*)\\s*:\\s*public\\s*Component\\s*<\\s*\\w*\\s*>");
  const char* target = str_configIn.c_str();
  for (auto it = cregex_iterator(target, target + strlen(target), rx_compNameFinder);
       it != cregex_iterator();
       ++it) {
    cmatch match = *it;
    compTypes.push_back({match[1].str()});
  }
  
  for (auto type : compTypes) {
    std::cout << type.name << std::endl;
  }
  
  regex rx_compDecls("(  \\/\\/ COMPONENT DECLARATIONS APPEAR HERE)");
  regex rx_compEnums("    \\/\\/ COMPONENT TYPE ENUMERATORS APPEAR HERE");
  regex rx_numCompTypes("  \\/\\/ NUMBER OF COMPONENT TYPES APPEARS HERE");
  string str_compsHOut = regex_replace(str_compsHIn, rx_compDecls, "");
  str_compsHOut = regex_replace(str_compsHOut, rx_compEnums, "");
  str_compsHOut = regex_replace(str_compsHOut, rx_numCompTypes, "");
  
  string str_compsCOut = str_compsCIn;
  string str_stateHOut = str_stateHIn;
  string str_stateCOut = str_stateCIn;
  
  
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
