#include "Parser/include/Common/Parser.h"
#include "Logger/include/Logger.h"

Parser::Parser(const string& parseFile) : xmlInputFile_(parseFile){};

XMLError Parser::CheckAttributeExists(XMLElement* pElem, string name) {
  XMLError ret = XML_SUCCESS;

  if (NULL == pElem->Attribute(name.c_str())) {
    LOG_WARNING("Attribute %s does not exist", name.c_str());
    ret = XML_NO_ATTRIBUTE;
  }
  return ret;
}

DeviceFamilyParser::DeviceFamilyParser(const string& parseFile)
    : Parser(parseFile){};

XMLError DeviceFamilyParser::Run(DeviceFamily** ppDevFamily) {
  XMLDocument cidFile;
  XMLElement* pElement;
  XMLError ret = XML_NO_ATTRIBUTE;

  if (nullptr == ppDevFamily) {
    LOG_ERROR("pDevFamily must not be nullptr");
  }

  ret = cidFile.LoadFile(xmlInputFile_.c_str());

  if (ret != XML_SUCCESS) {
    LOG_ERROR("Can't load source xml file: %s", xmlInputFile_.c_str());
    return ret;
  }

  XMLElement* pRoot = cidFile.FirstChildElement("Device");
  if (pRoot != nullptr) {
    if ((XML_SUCCESS != CheckAttributeExists(pRoot, "FamilyName")) ||
        (XML_SUCCESS != CheckAttributeExists(pRoot, "ApiVersion")) ||
        (XML_SUCCESS != CheckAttributeExists(pRoot, "Behavior")) ||
        (XML_SUCCESS != CheckAttributeExists(pRoot, "Group")) ||
        (XML_SUCCESS != CheckAttributeExists(pRoot, "DeviceType"))) {
      ret = XML_NO_ATTRIBUTE;
      LOG_ERROR("Attribute must be set here!");
      return ret;
    }

    XMLElement* pElement =
        pRoot->FirstChildElement("CompatibleMembers")->FirstChildElement();

    if (nullptr == pElement) {
      LOG_ERROR("Must contain CompatibleMembers node");
      ret = XML_NO_ATTRIBUTE;
      return ret;
    } else {
      if (XML_SUCCESS != CheckAttributeExists(pElement, "Name")) {
        ret = XML_NO_ATTRIBUTE;
        LOG_ERROR("Attribute must be set here!");
        return ret;
      }
    }

    *ppDevFamily = new DeviceFamily();

    (*ppDevFamily)->SetName(pRoot->Attribute("FamilyName"));
    (*ppDevFamily)->SetVersion(pRoot->Attribute("ApiVersion"));
    (*ppDevFamily)->SetBehavior(pRoot->Attribute("Behavior"));

    (*ppDevFamily)->SetGroup(pRoot->Attribute("Group"));
    (*ppDevFamily)->SetFamilyType(pRoot->Attribute("DeviceType"));

    while (pElement != nullptr) {
      (*ppDevFamily)->AddFamilyMember(pElement->Attribute("Name"));
      pElement = pElement->NextSiblingElement();
    }
  }
  return XML_SUCCESS;
}

