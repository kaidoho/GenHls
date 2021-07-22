#pragma once
#include <string>
#include <vector>
#include "Parser/include/Common/Parser.h"
#include "Device/include/Common/DeviceFamily.h"
#include "Device/include/CoLa/CoLaDevice.h"
#include "Device/include/CoLa/CoLaMethod.h"
#include "Device/include/CoLa/CoLaVariable.h"

#include "TinyXML2/include/tinyxml2.h"

using namespace std;
using namespace tinyxml2;
class CoLaDeviceParser : public Parser
{
 public:
  CoLaDeviceParser(const string& parseFile);
  ~CoLaDeviceParser(){};

  XMLError Run(DeviceFamily** ppDevFamily);

 private:
  XMLError parseInterfaces(XMLElement* pInterfaces);
  XMLError parseDefaultInterface(XMLElement* pInterfaces);
  XMLError parseUserLevels(XMLElement* pUserLevels);
  XMLError parseUserTypes(XMLElement* pUserTypes);
  XMLError parseVariables(XMLElement* pVariables);
  XMLError parseMethods(XMLElement* pMethods);

  GenericCoLaTypeContainer* parseType(XMLElement* pType,const string& localName,
                                       uint32_t lvl);
  GenericCoLaTypeContainer* parseStruct(XMLElement* pType,
                                        const string& localName,
                                         uint32_t lvl);
  GenericCoLaTypeContainer* parseArray(XMLElement* pType,
                                       const string& localName,
                                        uint32_t lvl);

  GenericCoLaTypeContainer* parseBasicType(XMLElement* pType,
                                           const string& localName,
                                           uint32_t lvl);
  GenericCoLaTypeContainer* parseUserType(XMLElement* pType,
                                          const string& localName,
                                           uint32_t lvl);

  GenericCoLaTypeContainer* BasicTypeFactory(XMLElement* pType,
                                             const string& localName);

 private:
  CoLaDevice* pDev_;
};