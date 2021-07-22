#include "Parser/include/CoLa/CoLaDeviceParser.h"
#include "Logger/include/Logger.h"
#include <algorithm>
#include <filesystem>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace std::filesystem;

//===========================================================================
//===========================================================================
CoLaDeviceParser::CoLaDeviceParser(const string& parseFile)
    : Parser(parseFile) {}

//===========================================================================
//===========================================================================
XMLError CoLaDeviceParser::Run(DeviceFamily** ppDevFamily) {
  XMLDocument cidFile;
  XMLError eResult = XML_SUCCESS;
  XMLElement* pElement;

  if (ppDevFamily == nullptr) {
    LOG_ERROR("Device Family pointer must not be nullptr");
    return XML_ERROR_PARSING_ELEMENT;
  }
  eResult = cidFile.LoadFile(xmlInputFile_.c_str());

  if (eResult != XML_SUCCESS) {
    LOG_ERROR("Can't load source xml file.");
    return eResult;
  }
  pDev_ = new CoLaDevice();
  pDev_->SetDeviceName((*ppDevFamily)->GetName());
  (*ppDevFamily)->SetParsedDevice(static_cast<Device*>(pDev_));

  XMLElement* pRoot = cidFile.FirstChildElement("Device");

  if (pRoot != nullptr) {
    //-----------------------------------------------
    // Parse Interfaces
    XMLElement* pElement =
        pRoot->FirstChildElement("CommunicationInterfaces");
    if (pElement != nullptr) {
      eResult = parseInterfaces(pElement);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error: parsing interfaces");
        return eResult;
      }
      eResult = parseDefaultInterface(pElement);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error: no default interface given");
        return eResult;
      }
    } else {
      return XML_ERROR_PARSING_ELEMENT;
    }
    // parse default functions
    pElement = pRoot->FirstChildElement("RebootFunction");
    if (pElement != nullptr) {
      const char* RebootName;
      int sleepTime;
      eResult = pElement->QueryStringAttribute("Name", &RebootName);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error no name given for RebootFunction");
        return eResult;
      }
      eResult = pElement->QueryIntAttribute("WaitMilliseconds", &sleepTime);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error no WaitMilliseconds given");
        return eResult;
      } else {
        pDev_->SetRebootFuncName(string(RebootName), sleepTime);
      }
    }
    pElement = pRoot->FirstChildElement("StoreParameter");
    if (pElement != nullptr) {
      const char* StoreName;
      int sleepTime;
      eResult = pElement->QueryStringAttribute("Name", &StoreName);

      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error no name given for StoreParameter");
        return eResult;
      }
      eResult = pElement->QueryIntAttribute("WaitMilliseconds", &sleepTime);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error no WaitMilliseconds given");
        return eResult;
      } else {
        pDev_->SetStoreParamFuncName(string(StoreName), sleepTime);
      }
    }
    pElement = pRoot->FirstChildElement("FactoryDefaults");
    if (pElement != nullptr) {
      const char* FactoryDefaultsName;
      int sleepTime;
      eResult = pElement->QueryStringAttribute("Name", &FactoryDefaultsName);

      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error no name given for FactoryDefaults");
        return eResult;
      }
      eResult = pElement->QueryIntAttribute("WaitMilliseconds", &sleepTime);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error no WaitMilliseconds given");
        return eResult;
      } else {
        pDev_->SetFactoryDefaultFuncName(string(FactoryDefaultsName),
                                          sleepTime);
      }
    }

    //-----------------------------------------------
    // parse user levels
    pElement = pRoot->FirstChildElement("UserLevels");
    if (pElement != nullptr) {
      eResult = parseUserLevels(pElement);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error parsing user levels");
        return eResult;
      }
    }
    //-----------------------------------------------
    // parse user types
    pElement = pRoot->FirstChildElement("DataTypes");
    if (pElement != nullptr) {
      eResult = parseUserTypes(pElement);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error parsing user types");
        return eResult;
      }
    }
    //-----------------------------------------------
    // parse variales
    pElement = pRoot->FirstChildElement("Variables");
    if (pElement != nullptr) {
      eResult = parseVariables(pElement);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error parsing variables");
        
        return eResult;
      }
    }
    //-----------------------------------------------
    // parse functions
    pElement = pRoot->FirstChildElement("Functions");
    if (pElement != nullptr) {
      eResult = parseMethods(pElement);
      if (eResult != XML_SUCCESS) {
        LOG_ERROR("Error parsing methods");

        return eResult;
      }
    }
  }
  
  return eResult;
}

//===========================================================================
//===========================================================================
XMLError CoLaDeviceParser::parseUserLevels(XMLElement* pUserLevels) {
  XMLError eResult = XML_ERROR_PARSING_ELEMENT;
  XMLElement* pElement = pUserLevels->FirstChildElement("Level");
  while (pElement != nullptr) {
    const char* name;
    const char* pw;

    eResult = pElement->QueryStringAttribute("Name", &name);
    if (XML_NO_ATTRIBUTE == eResult) {
      LOG_ERROR("User level has no attribute Name");
      return eResult;
    }
    eResult = pElement->QueryStringAttribute("Password", &pw);
    if (XML_NO_ATTRIBUTE == eResult) {
      LOG_ERROR("User level has no attribute Password");
      return eResult;
    }
    CoLaUserLevel* tmpLvl = new CoLaUserLevel(
        pElement->Attribute("Name"), pElement->Attribute("Password"));

    pDev_->AddUserLevel(tmpLvl);
    pElement = pElement->NextSiblingElement();
  }
  return eResult;
}

//===========================================================================
//===========================================================================
XMLError CoLaDeviceParser::parseInterfaces(XMLElement* pInterfaces) {
  XMLError eResult = XML_ERROR_PARSING_ELEMENT;
  XMLElement* pElement = pInterfaces->FirstChildElement("Interface");

  if (pElement == nullptr) {
    LOG_ERROR("Error device has no interfaces");
    return eResult;
  }
  while (pElement != nullptr) {
    const char* name;
    const char* type;
    const char* addressing;
    const char* switchVar;
    const char* switchVarValue;
    int32_t portOrBaudrate;

    eResult = pElement->QueryStringAttribute("Name", &name);
    if (XML_NO_ATTRIBUTE == eResult) {
      LOG_ERROR("Error attribute Name");
      return eResult;
    }
    eResult = pElement->QueryStringAttribute("Type", &type);
    if (XML_NO_ATTRIBUTE == eResult) {
      LOG_ERROR("Error attribute Type");
      return eResult;
    }
    eResult = pElement->QueryIntAttribute("Port", &portOrBaudrate);
    if (XML_NO_ATTRIBUTE == eResult) {
      eResult = pElement->QueryIntAttribute("Baudrate", &portOrBaudrate);
      if (XML_NO_ATTRIBUTE == eResult) {
        LOG_ERROR("No baudrate given");
        return eResult;
      }
    }

    SensorInterface* pTmp =
        new SensorInterface(pElement->Attribute("Name"),
                            pElement->Attribute("Type"), portOrBaudrate);

    XMLElement* pSubElement = pElement->FirstChildElement("Protocol");
    while (pSubElement != nullptr) {
      if (NULL == pSubElement) {
        LOG_ERROR( "Can't find a protocol for this interface");
        return XML_ERROR_PARSING_ELEMENT;
      }
      eResult = pSubElement->QueryStringAttribute("Type", &type);
      if (XML_NO_ATTRIBUTE == eResult) {
        return eResult;
      }
      eResult = pSubElement->QueryStringAttribute("Addressing", &addressing);
      SensorInterfaceProtocol* pProto;
      if (XML_NO_ATTRIBUTE == eResult) {
        pProto = (new SensorInterfaceProtocol(pSubElement->Attribute("Type"),
                                              pSubElement->Attribute("")));
      } else {
        pProto =
            (new SensorInterfaceProtocol(pSubElement->Attribute("Type"),
                                         pSubElement->Attribute("Addressing")));
      }

      eResult = pSubElement->QueryStringAttribute("SwitchVar", &switchVar);
      if (XML_NO_ATTRIBUTE != eResult) {
        pProto->SetSwitchVar(string(switchVar));
        eResult = pSubElement->QueryStringAttribute("SwitchVarValue",
                                                    &switchVarValue);
        if (XML_NO_ATTRIBUTE != eResult) {
          pProto->SetSwitchVarValue(string(switchVarValue));
        } else {
          LOG_ERROR("Switch variable given but no value");
          return XML_ERROR_PARSING_ELEMENT;
        }
      }

      eResult = pSubElement->QueryStringAttribute("Preferred", &addressing);
      if (XML_NO_ATTRIBUTE != eResult) {
        if (0 == std::string(addressing).compare("True")) {
          pProto->SetDefaultInterface();
        }
      } else {
        eResult = XML_SUCCESS;
      }
      pTmp->AddProtocol(pProto);
      pSubElement = pSubElement->NextSiblingElement();
    }

    pDev_->AddInterface(pTmp);

    pElement = pElement->NextSiblingElement("Interface");
  }

  return eResult;
}

//===========================================================================
//===========================================================================
XMLError CoLaDeviceParser::parseDefaultInterface(XMLElement* pInterfaces) {
  XMLError eResult = XML_ERROR_PARSING_ELEMENT;
  XMLElement* pElement = pInterfaces->FirstChildElement("DefaultInterface");

  if (pElement != nullptr) {
    const char* name;
    eResult = pElement->QueryStringAttribute("Name", &name);
    if (XML_NO_ATTRIBUTE == eResult) {
      return eResult;
    }
    pDev_->SetDefaultInterface(pElement->Attribute("Name"));
  }
  return eResult;
}

//===========================================================================
//===========================================================================
XMLError CoLaDeviceParser::parseUserTypes(XMLElement* pUserTypes) {
  XMLError eResult = XML_SUCCESS;
  XMLElement* pElement = pUserTypes->FirstChildElement("TypeDefinition");
  while (pElement != nullptr) {
    const char* name;

    eResult = pElement->QueryStringAttribute("Name", &name);
    LOG_INFO("Found user type: %s", name );
    XMLElement* pSubElement = pElement->FirstChildElement();
    GenericCoLaTypeContainer* pType = parseType(pSubElement, string(name), 0);
    if (NULL != pType) {
      delete pType;
      // pDev_->deviceDataTypes_.push_back(pType);
    }
    pElement = pElement->NextSiblingElement();
  }
  return eResult;
}

//===========================================================================
//===========================================================================
GenericCoLaTypeContainer* CoLaDeviceParser::BasicTypeFactory(
    XMLElement* pType, const string& localName) {
  string typeName = pType->Value();
  string max, min;

  GenericCoLaTypeContainer* pRetType = NULL;
  if (0 == typeName.compare("SInt")) {
    BaseTypeContainer<int8_t>* pX = new BaseTypeContainer<int8_t>;
    pX->SetBaseTypeName("int8_t");
    pX->SetRange(pType);

    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("USInt")) {
    BaseTypeContainer<uint8_t>* pX = new BaseTypeContainer<uint8_t>;
    pX->SetBaseTypeName("uint8_t");
    pX->SetRange(pType);

    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("Int")) {
    BaseTypeContainer<int16_t>* pX = new BaseTypeContainer<int16_t>;
    pX->SetBaseTypeName("int16_t");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("UInt")) {
    BaseTypeContainer<uint16_t>* pX = new BaseTypeContainer<uint16_t>;
    pX->SetBaseTypeName("uint16_t");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("DInt")) {
    BaseTypeContainer<int32_t>* pX = new BaseTypeContainer<int32_t>;
    pX->SetBaseTypeName("int32_t");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("UDInt")) {
    BaseTypeContainer<uint32_t>* pX = new BaseTypeContainer<uint32_t>;
    pX->SetBaseTypeName("uint32_t");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("LInt")) {
    BaseTypeContainer<int64_t>* pX = new BaseTypeContainer<int64_t>;
    pX->SetBaseTypeName("int64_t");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("ULInt")) {
    BaseTypeContainer<uint64_t>* pX = new BaseTypeContainer<uint64_t>;
    pX->SetBaseTypeName("uint64_t");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("Real")) {
    BaseTypeContainer<float>* pX = new BaseTypeContainer<float>;
    pX->SetBaseTypeName("float");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("LReal")) {
    BaseTypeContainer<double>* pX = new BaseTypeContainer<double>;
    pX->SetBaseTypeName("double");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("Bool")) {
    BaseTypeContainer<uint8_t>* pX = new BaseTypeContainer<uint8_t>;
    pX->SetBaseTypeName("uint8_t");
    pX->SetRange(pType);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if (0 == typeName.compare("String")) {
    StringContainer* pX = new StringContainer(pType, localName);
    pX->SetLocalName(pX->GetBaseTypeName());
    pDev_->AddType(pX);

    UserTypeContainer* pU = new UserTypeContainer();
    pU->SetLocalName(localName);
    pU->SetBaseTypeName(pX->GetBaseTypeName());

    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pU);
  } else if (0 == typeName.compare("XByte")) {
    XByteContainer* pX = new XByteContainer(pType, localName);

    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pX);
  } else if ((0 == typeName.compare("Enum8")) ||
             (0 == typeName.compare("Enum16"))) {
    EnumContainer* pE = new EnumContainer(pType, typeName, localName);
    pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pE);
  }

  else {
    LOG_ERROR("Ups");
  }
  pRetType->SetLocalName(localName);
  return pRetType;
}

//===========================================================================
//===========================================================================
GenericCoLaTypeContainer* CoLaDeviceParser::parseStruct(XMLElement* pType,
                                                        const string& localName,
                                               uint32_t lvl) {
  GenericCoLaTypeContainer* pRetType = NULL;
  string type = pType->Value();

  if (0 == type.compare("Struct")) {
    XMLElement* pSubElement = pType->FirstChildElement();
    LOG_DEBUG("Begin Struct");
    StructContainer* pS = new StructContainer();

    while (NULL != pSubElement) {
      pS->AddElement(parseType(pSubElement, localName, lvl));
      pSubElement = pSubElement->NextSiblingElement();
    }
    LOG_DEBUG("Struct done");
    return dynamic_cast<GenericCoLaTypeContainer*>(pS);
  }
  return pRetType;
}

//===========================================================================
//===========================================================================
GenericCoLaTypeContainer* CoLaDeviceParser::parseArray(XMLElement* pType,
                                                       const string& localName,
                                                       uint32_t lvl) {
  GenericCoLaTypeContainer* pRetType = NULL;
  ArrayContainer* pA = new ArrayContainer(pType, localName);
  XMLElement* pSubElement = pType->FirstChildElement();
  pA->AddElement(
      parseType(pSubElement, localName + "_" + pA->GetLocalName(), lvl));

  pRetType = dynamic_cast<GenericCoLaTypeContainer*>(pA);

  return pRetType;
}

//===========================================================================
//===========================================================================
GenericCoLaTypeContainer* CoLaDeviceParser::parseBasicType(XMLElement* pType, const string& localName,
                                                  uint32_t lvl) {
  const char* name;
  XMLError eResult;
  eResult = pType->QueryStringAttribute("Name", &name);

  if (eResult != XML_SUCCESS) {
    XMLElement* pSubElement = pType->FirstChildElement();
    return BasicTypeFactory(pSubElement, localName);
  } else {
    XMLElement* pSubElement = pType->FirstChildElement();
    return BasicTypeFactory(pSubElement, string(name));
  }
}

//===========================================================================
//===========================================================================
GenericCoLaTypeContainer* CoLaDeviceParser::parseUserType(
    XMLElement* pType, const string& localName,
    uint32_t lvl) {
  const char* name;
  const char* basictypename;
  XMLError eResult;

  eResult = pType->QueryStringAttribute("TypeName", &basictypename);

  if (eResult != XML_SUCCESS) {

    LOG_ERROR("Error user type without name");
    return NULL;
  }

  eResult = pType->QueryStringAttribute("Name", &name);
  if (eResult != XML_SUCCESS) {
    LOG_ERROR("Warning user type without name");
    name = basictypename;
  }

  UserTypeContainer* pX = new UserTypeContainer();
  pX->SetLocalName(string(name));
  pX->SetBaseTypeName(string(basictypename) + "_t");

  return dynamic_cast<GenericCoLaTypeContainer*>(pX);
}

//===========================================================================
//===========================================================================
GenericCoLaTypeContainer* CoLaDeviceParser::parseType(XMLElement* pType,
                                             const string& localName, uint32_t lvl) {
  GenericCoLaTypeContainer* pRetType = NULL;
  XMLError eResult;
  string type = pType->Value();

  LOG_DEBUG("Type: %s", type.c_str());

  while (NULL != pType) {
    if (0 == type.compare("Struct")) {
      GenericCoLaTypeContainer* pTmp;
      UserTypeContainer* pX;
      pTmp = parseStruct(pType, localName, lvl + 1);

      const char* name;
      eResult = pType->QueryStringAttribute("Name", &name);

      if (eResult != XML_SUCCESS) {
        pX = new UserTypeContainer();
        pX->SetLocalName(localName);
        if (lvl == 0) {
          pX->SetBaseTypeName(localName + "_t");
        } else {
          pX->SetBaseTypeName(localName + "_struct_t");
        }
      } else {
        pX = new UserTypeContainer();
        pX->SetLocalName(string(name));
        pX->SetBaseTypeName(string(name) + "_struct_t");
      }
      pTmp->SetLocalName(pX->GetLocalName());
      pTmp->SetBaseTypeName(pX->GetBaseTypeName());
      if (pTmp != NULL) {
        pDev_->AddType(pTmp);
      }

      return dynamic_cast<UserTypeContainer*>(pX);
    } else if (0 == type.compare("Array")) {
      return parseArray(pType, localName, lvl + 1);
    } else if (0 == type.compare("BasicType")) {
      return parseBasicType(pType, localName, lvl);
    } else if (0 == type.compare("UserType")) {
      return parseUserType(pType, localName, lvl);
    }
    pType = pType->NextSiblingElement();
  }

  return pRetType;
}

//===========================================================================
//===========================================================================
XMLError CoLaDeviceParser::parseVariables(XMLElement* pVariables) {
  XMLError eResult = XML_SUCCESS;
  XMLElement* pElement = pVariables->FirstChildElement("Variable");
  while (pElement != nullptr) {
    const char* name;
    const char* comname;
    const char* rwDir;
    const char* viewAccess;
    const char* writeAccess;

    int32_t idx;
    int32_t edx;

    eResult = pElement->QueryStringAttribute("Name", &name);

    LOG_INFO("Parsing variable: %s" , name );

    if (XML_NO_ATTRIBUTE == eResult) {
      return eResult;
    }
    eResult = pElement->QueryStringAttribute("RWDirection", &rwDir);
    if (XML_NO_ATTRIBUTE == eResult) {
      return eResult;
    }
    eResult = pElement->QueryStringAttribute("ViewAccessRef", &viewAccess);
    if (XML_NO_ATTRIBUTE == eResult) {
      viewAccess = "Run";
    }
    eResult = pElement->QueryStringAttribute("WriteAccessRef", &writeAccess);
    if (XML_NO_ATTRIBUTE == eResult) {
      writeAccess = "Run";
    }

    eResult = pElement->QueryIntAttribute("SVIdx", &idx);
    if (XML_NO_ATTRIBUTE == eResult) {
      return eResult;
    }
    CoLaVariable* tmpVar =
        new CoLaVariable(name, rwDir, viewAccess, writeAccess, idx);

    eResult = pElement->QueryStringAttribute("CommunicationName", &comname);
    if (XML_NO_ATTRIBUTE == eResult) {
      eResult = XML_SUCCESS;
      tmpVar->setComName(name);
    } else {
      tmpVar->setComName(comname);
    }
    eResult = pElement->QueryIntAttribute("SEIdx", &edx);
    if (XML_NO_ATTRIBUTE != eResult) {
      tmpVar->setEventIndex(edx);
    } else {
      eResult = XML_SUCCESS;
    }

    XMLElement* pSubElement = pElement->FirstChildElement();

    tmpVar->setType(parseType(pSubElement, string(name), 0));

    pDev_->AddVariable(tmpVar);

    pElement = pElement->NextSiblingElement();
  }
  return eResult;
}

//===========================================================================
//===========================================================================
XMLError CoLaDeviceParser::parseMethods(XMLElement* pMethods) {
  XMLError eResult = XML_SUCCESS;
  XMLElement* pElement = pMethods->FirstChildElement("Function");
  while (pElement != nullptr) {
    const char* name;
    const char* comname;
    const char* accessLevel;
    int32_t midx;

    eResult = pElement->QueryStringAttribute("Name", &name);
    if (XML_NO_ATTRIBUTE == eResult) {
      return eResult;
    }
    eResult =
        pElement->QueryStringAttribute("InvocationAccessRef", &accessLevel);
    if (XML_NO_ATTRIBUTE == eResult) {
      accessLevel = "Run";
    }

    eResult = pElement->QueryIntAttribute("SMIdx", &midx);
    if (XML_NO_ATTRIBUTE == eResult) {
      return eResult;
    }
    CoLaMethod* tmpMethod = new CoLaMethod(name, accessLevel, midx);

    eResult = pElement->QueryStringAttribute("CommunicationName", &comname);
    if (XML_NO_ATTRIBUTE == eResult) {
      eResult = XML_SUCCESS;
      tmpMethod->setComName(name);
    } else {
      tmpMethod->setComName(comname);
    }

    XMLElement* pParams = pElement->FirstChildElement("Parameter");
    if (pParams != 0) {
      tmpMethod->SetArgType(
          parseType(pParams->FirstChildElement(), string(name), 0));
    }

    XMLElement* pReturn = pElement->FirstChildElement("ReturnValue");
    if (pReturn != 0) {
      tmpMethod->SetReturnType(
          parseType(pReturn->FirstChildElement(), string(name), 0));
    }

    pDev_->AddMethod(tmpMethod);

    pElement = pElement->NextSiblingElement();
  }
  return eResult;
}
