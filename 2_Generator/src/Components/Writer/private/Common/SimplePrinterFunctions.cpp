#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/Common/Writer.h"

using namespace std;

void Writer::supressMsvcWarning(std::ofstream* pFile, uint32_t number) {
  *pFile << "#ifdef _MSC_VER" << endl;
  *pFile << "#pragma warning(disable : " + to_string(number) + ")" << endl;
  *pFile << "#endif" << endl;
}

void Writer::addBlankLine(std::ofstream* pFile) { *pFile << endl; }

void Writer::writeNoticeHeader(std::ofstream* pFile) {
  *pFile << "//===========================================" << endl;
  *pFile << "//  Generated file - do not modify" << endl;
  *pFile << "//===========================================" << endl;
}

void Writer::addSystemIncludeFile(ofstream* file, string systemheader) {
  *file << "#include <" + systemheader + ">" << endl;
}

void Writer::addLocalIncludeFile(ofstream* file, string suffix) {
  *file << "#include \"API/Skeleton/" + pDevFamily_->GetGroup() + "/" +
               pDevFamily_->GetName() + "/" +
               pDevFamily_->pDevBase_->GetNameVersionCombined() + "/include/" +
               pDevFamily_->pDevBase_->GetDeviceName() + suffix + "\""
        << endl;
}

void Writer::addProjectIncludeFile(ofstream* file, string filename) {
  *file << "#include \"" + filename + "\"" << endl;
}

void Writer::createSourceHeader(ofstream* file) {
  writeNoticeHeader(file);
  addBlankLine(file);
}

void Writer::createHeaderHeader(ofstream* file) {
  writeNoticeHeader(file);
  addBlankLine(file);
  *file << "#pragma once" << endl;
}

void Writer::openNamespace(std::ofstream* file, std::string Namespace) {
  *file << "namespace " + Namespace << endl;
  *file << "{" << endl;
}

void Writer::closeNamespace(std::ofstream* file, std::string Namespace) {
  *file << "} // namespace " + Namespace << endl;
}

void Writer::openClass(std::ofstream* file, std::string text) {
  *file << "class " + text << endl;
  *file << "{" << endl;
  *file << "public:" << endl;
}

void Writer::closeClass(std::ofstream* file, std::string text) {
  *file << "}; //" << text;
  addBlankLine(file);
}

void Writer::openClassMethod(std::ofstream* file, std::string className,
                             std::string methodName) {
  *file << className << "::" << methodName << endl;
  *file << "\t{" << endl;
}

void Writer::closeClassMethod(std::ofstream* file) { *file << "\t}" << endl; }

