#pragma once
#include <string>
#include <vector>
#include "TinyXML2/include/tinyxml2.h"

#include "Device/include/Common/DeviceFamily.h"

using namespace std;
using namespace tinyxml2;


class Parser {
 public:
  Parser(const string& parseFile);
  ~Parser(){};
  
  virtual XMLError Run(DeviceFamily** ppDevFamily) = 0;
  XMLError CheckAttributeExists(XMLElement* pElem,  string name);

 protected:
  string xmlInputFile_;
};



class DeviceFamilyParser : Parser {
 public:
  DeviceFamilyParser(const string& parseFile);
  ~DeviceFamilyParser(){};


  XMLError Run(DeviceFamily** ppDevFamily);

};

Parser* createTypeParser(DeviceFamily* pDevFamily, const string& parseFile);