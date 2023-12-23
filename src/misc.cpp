#include "misc.h"

#include <iostream>
#include <fstream>
#include <sstream>

void writeFile(const char* fileName, int numToWrite) {

    std::ofstream writeStream(fileName);

    std::ostringstream writeString;
    writeString << numToWrite;

    writeStream << writeString.str();
    writeStream.close();
}
int readFile(const char* fileName) {
    std::fstream readStream(fileName, std::ios_base::in);
    int readVal;
    readStream >> readVal;
    return readVal;
}
