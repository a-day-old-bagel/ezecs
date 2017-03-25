//
// Created by Galen on 2017-03-24.
//

#include <stdio.h>

#define TAB "    "

int main (int argc, char *argv[]) {
    // arguments provided by CMake
    if (argc < 3) { return 1; }

    FILE *headerOut = fopen(argv[2], "w");
    if (!headerOut) { return 1; }
    FILE *sourceOut = fopen(argv[3], "w");
    if (!sourceOut) { return 1; }

    fprintf(headerOut, "/*\n * EZECS - The E-Z Entity Component System\n * Header generated using %s\n */\n", argv[1]);
    fprintf(sourceOut, "/*\n * EZECS - The E-Z Entity Component System\n * Source generated using %s\n */\n", argv[1]);
    
    fprintf(headerOut, "\n"
            "#include <iostream>\n"
            "namespace ezecs {\n"
            TAB "void testFunction() {\n"
            TAB TAB "printf(\"generated code works!\\n\");\n"
            TAB "}\n"
            "}\n"
    );
    
    fclose(headerOut);
    fclose(sourceOut);
    
    return 0;
}

#undef TAB
