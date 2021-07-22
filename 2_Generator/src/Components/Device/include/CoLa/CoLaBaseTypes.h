#pragma once
#include <string>
#include <typeinfo>
#include <vector>

#include <iostream>
#include "TinyXML2/include/tinyxml2.h"
using namespace std;
using namespace tinyxml2;

enum TypeClass {
  UNSET,
  STRUCT_CONTAINER,
  USER_TYPE_CONTAINER,
  ARRAY_CONTAINER,
  CONCRETE_TYPE_CONTAINER,
  STRING_CONTAINER,
  XBYTE_CONTAINER,
  ENUM_CONTAINER,
};

enum BasicConrete {
  COLA_SINT = 0,
  COLA_USINT,
  COLA_INT,
  COLA_UINT,
  COLA_DINT,
  COLA_UDINT,
  COLA_LINT,
  COLA_ULINT,
  COLA_REAL,
  COLA_LREAL,
  COLA_BOOL,
  COLA_STRING,
  COLA_ENUM8,
  COLA_ENUM16,
  COLA_MAX_CONCRET_TYPES,
};

typedef struct {
  std::string ChoiceName;
  int32_t ChoiceValue;
} enumKeypair_t;

typedef struct {
  std::string Type;
  std::string Name;
  int32_t Width;
} XByteType_t;

class GenericCoLaTypeContainer {
 public:
  GenericCoLaTypeContainer()
      : localName_(""),
        baseTypeName_(""),
        comment_(""),
        typeClass_(UNSET)

            {};
  ~GenericCoLaTypeContainer(){};

  TypeClass GetTypeClass(void) { return typeClass_; }
  void SetTypeClass(TypeClass tc) { typeClass_ = tc; }

  void SetLocalName(const string& name) { localName_ = name; }
  string GetLocalName() { return localName_; }
  void SetBaseTypeName(const string& name) { baseTypeName_ = name; }

  virtual string GetBaseTypeName() { return baseTypeName_; };

  void SetComment(const string& com) { comment_ = com; }

  virtual string GetComment() { return comment_; };

  vector<GenericCoLaTypeContainer*> GetChildren() { return children_; }

  virtual std::string GetMin(void) { return ""; }
  virtual std::string GetMax(void) { return ""; }
  virtual std::string GetDefault(void) { return ""; }

 protected:
  vector<GenericCoLaTypeContainer*> children_;

  string localName_;
  string baseTypeName_;
  string comment_;
  TypeClass typeClass_;
};

class StructContainer : public GenericCoLaTypeContainer {
 public:
  StructContainer() { typeClass_ = STRUCT_CONTAINER; };

  ~StructContainer(){};

  void AddElement(GenericCoLaTypeContainer* pElem) {
    children_.push_back(pElem);
  };
};

class ArrayContainer : public GenericCoLaTypeContainer {
 public:
  ArrayContainer(XMLElement* pType, const string& localName) {
    typeClass_ = ARRAY_CONTAINER;
    XMLError eResult;
    int32_t nElems = 0;
    const char* isFixedLength;
    const char* localAddrName;
    isFixLengthArray_ = false;

    eResult = pType->QueryStringAttribute("Name", &localAddrName);
    if (eResult == XML_SUCCESS) {
      localName_ = string(localAddrName);
    } else {
      localName_ = localName + "_array";
    }

    eResult = pType->QueryIntAttribute("Length", &nElems);
    if (eResult != XML_SUCCESS) {
      nElem_ = 0;
      cout << "No Lenght given by array" << endl;
    } else {
      nElem_ = nElems;
    }

    eResult = pType->QueryStringAttribute("FixedLength", &isFixedLength);

    if (eResult == XML_SUCCESS) {
      if (0 == string(isFixedLength).compare("True")) {
        isFixLengthArray_ = true;
      }
    }

  };

  ~ArrayContainer(){};

  void AddElement(GenericCoLaTypeContainer* pElem) {
    children_.push_back(pElem);
  };

  void SetArraySize(uint32_t nElem) { nElem_ = nElem; }
  int32_t GetArraySize() { return nElem_; }
  string GetBaseTypeName() { return children_[0]->GetBaseTypeName(); }
  bool IsFlexArray() { return !isFixLengthArray_; }

 private:
  int32_t nElem_;
  bool isFixLengthArray_;
};

template <class T>
class BaseTypeContainer : public GenericCoLaTypeContainer {
 public:
  BaseTypeContainer() { typeClass_ = CONCRETE_TYPE_CONTAINER; };

  ~BaseTypeContainer(){};
  void SetRange(XMLElement* pElem) {
    XMLError eResult;
    const char* minValue;
    const char* maxValue;
    const char* defaultValue;

    eResult = pElem->QueryStringAttribute("MinValue", &minValue);
    if (XML_SUCCESS == eResult) {
      minValue_ = string(minValue);
    } else {
      minValue_ = "UNSET";
    }

    if (0 == minValue_.compare("-2147483648")) {
      minValue_ = "SSBL_LONG_MIN";
    } else if (0 == minValue_.compare("-9223372036854775808")) {
      minValue_ = "SSBL_LLONG_MIN";
    }

    pElem->QueryStringAttribute("MaxValue", &maxValue);
    if (XML_SUCCESS == eResult) {
      maxValue_ = string(maxValue);
    } else {
      maxValue_ = "UNSET";
    }

    if (0 == maxValue_.compare("-2147483648")) {
      maxValue_ = "SSBL_LONG_MIN";
    } else if (0 == maxValue_.compare("-9223372036854775808")) {
      maxValue_ = "SSBL_LLONG_MIN";
    }

    pElem->QueryStringAttribute("DefaultValue", &defaultValue);
    if (XML_SUCCESS == eResult) {
      defaultValue_ = string(defaultValue);
    } else {
      defaultValue_ = minValue_;
    }

    if (0 == defaultValue_.compare("-2147483648")) {
      defaultValue_ = "SSBL_LONG_MIN";
    } else if (0 == defaultValue_.compare("-9223372036854775808")) {
      defaultValue_ = "SSBL_LLONG_MIN";
    }
  }

  std::string GetMin(void) { return minValue_; }
  std::string GetMax(void) { return maxValue_; }
  std::string GetDefault(void) { return defaultValue_; }

 private:
  std::string minValue_;
  std::string defaultValue_;
  std::string maxValue_;
};

class UserTypeContainer : public GenericCoLaTypeContainer {
 public:
  UserTypeContainer() { typeClass_ = USER_TYPE_CONTAINER; }
  ~UserTypeContainer(){};
};

class StringContainer : public GenericCoLaTypeContainer {
 public:
  StringContainer(XMLElement* pType, const string& localName) {
    localName_ = localName;
    typeClass_ = STRING_CONTAINER;

    XMLError eResult;
    int32_t nElems = 0;
    const char* isFixedLength;
    const char* localAddrName;
    isFixLength_ = false;

    eResult = pType->QueryStringAttribute("Name", &localAddrName);
    if (eResult == XML_SUCCESS) {
      localName_ = localAddrName;
    }

    eResult = pType->QueryIntAttribute("Length", &nElems);
    if (eResult != XML_SUCCESS) {
      nElem_ = 0;
      cout << "No Lenght given by array" << endl;
    } else {
      nElem_ = nElems;
    }

    eResult = pType->QueryStringAttribute("FixedLength", &isFixedLength);

    if (eResult == XML_SUCCESS) {
      if (0 == string(isFixedLength).compare("True")) {
        isFixLength_ = true;
      }
    }

  };

  ~StringContainer(){};

  string GetBaseTypeName() {
    string s;
    if (isFixLength_) {
      s = "FixString";
    } else {
      s = "FlexString";
    }
    s = s + to_string(nElem_);

    return s;
  }
  bool isFixLength() { return isFixLength_; }
  int32_t GetLength() { return nElem_; }

 private:
  int32_t nElem_;
  bool isFixLength_;
};

class XByteContainer : public GenericCoLaTypeContainer {
 public:
  XByteContainer(XMLElement* pType, const string& localName) {
    localName_ = localName;
    typeClass_ = XBYTE_CONTAINER;

    XMLError eResult;

    const char* localAddrName;

    eResult = pType->QueryStringAttribute("Name", &localAddrName);

    if (eResult == XML_SUCCESS) {
      localName_ = localAddrName;
    }

    XMLElement* pSubSubElement = pType->FirstChildElement();

    uint32_t widthInBits = 0;

    while (pSubSubElement != nullptr) {
      const char* xName;
      int32_t width;
      string xtype = pSubSubElement->Value();

      eResult = pSubSubElement->QueryStringAttribute("Name", &xName);

      if (xtype.compare("Bool") == 0) {
        width = 1;
      } else {
        for( const XMLAttribute* a = pSubSubElement->FirstAttribute(); a; a=a->Next()) 
        {
          cout << a->Name() << endl;
        }
        eResult = pSubSubElement->QueryIntAttribute("Width", &width);
      }
      XByteType_t* pX = new XByteType_t;

      pX->Name = xName;
      pX->Type = xtype;
      pX->Width = width;
      AddXByteStruct(pX);

      widthInBits = widthInBits + width;
      pSubSubElement = pSubSubElement->NextSiblingElement();
    }
    if (widthInBits % 8) {
      widthInBytes_ = widthInBits / 8 + 1;
    } else {
      widthInBytes_ = widthInBits / 8;
    }
  };

  ~XByteContainer(){};

  uint32_t GetWidthInBytes() { return widthInBytes_; }

  void AddXByteStruct(XByteType_t* v) { xByteList.push_back(v); };

  std::vector<XByteType_t*> GetXByteStructs() { return xByteList; };

 private:
  std::vector<XByteType_t*> xByteList;
  uint32_t widthInBytes_;
};

class EnumContainer : public GenericCoLaTypeContainer {
 public:
  EnumContainer(XMLElement* pType, const string& typeName,
                const string& localName) {
    localName_ = localName;
    typeClass_ = ENUM_CONTAINER;

    XMLError eResult;

    const char* localAddrName;

    eResult = pType->QueryStringAttribute("Name", &localAddrName);
    XMLElement* pSubElement = pType->FirstChildElement();
    string basicTypeType = pSubElement->Value();

    if ((0 == typeName.compare("Enum8")) || 0 == (typeName.compare("Enum16"))) {
      int32_t widthX = 16;
      if (0 == typeName.compare("Enum8")) {
        widthX = 8;
      }
      baseTypeName_ = typeName + "_t";
      width_ = widthX;

      XMLElement* pSubSubElement = pSubElement->FirstChildElement("Choice");

      while (pSubSubElement != nullptr) {
        const char* choiceName;
        int32_t choiceValue = 0;
        eResult = pSubSubElement->QueryStringAttribute("Name", &choiceName);
        eResult =
            pSubSubElement->QueryIntAttribute("PreAllocation", &choiceValue);

        enumKeypair_t* pPair = new enumKeypair_t;

        pPair->ChoiceName = choiceName;
        pPair->ChoiceValue = choiceValue;
        AddEnumKeypair(pPair);
        pSubSubElement = pSubSubElement->NextSiblingElement();
      }
    }
  };

  ~EnumContainer(){};

  uint32_t GetWidth() { return width_; }

  void AddEnumKeypair(enumKeypair_t* v) { enumKeys.push_back(v); };

 private:
  std::vector<enumKeypair_t*> enumKeys;
  uint32_t width_;
};
