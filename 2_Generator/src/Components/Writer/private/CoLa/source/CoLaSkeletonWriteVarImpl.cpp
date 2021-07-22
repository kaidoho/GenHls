#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/CoLa/CoLaSkeletonWriter.h"

//CoLaSkeletonWriter
using namespace std;

//===========================================================================
void unique_strings(std::vector<string>& vec) {
  uint32_t* cnts = new uint32_t[vec.size()];
  memset(cnts, 0, sizeof(uint32_t) * vec.size());
  uint32_t idx = 0;
  for (auto x : vec) {
    for (auto z : vec) {
      if (0 == z.compare(x)) {
        cnts[idx]++;
      }
    }
    idx++;
  }
  idx = 0;
  auto it = vec.begin();
  while (it != vec.end()) {
    if (cnts[idx] != 1) {
      it = vec.erase(it);
    } else {
      ++it;
    }
    idx++;
  }
}

//===========================================================================
bool checkIfStringIsUniqueInVector(string searchS, vector<string> elem) {
  for (auto x : elem) {
    if (0 == x.compare(searchS) && x.size() == searchS.size()) {
      return true;
    }
  }

  return false;
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableImplementationFile(const string& folder) {
  ofstream file;
  string FileString =
      folder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_Variables.cpp";

  file.open(FileString.c_str(), ios::out | ios::trunc);

  if (file.is_open()) {
    writeNoticeHeader(&file);
    addBlankLine(&file);

    addLocalIncludeFile(&file, "_Variables.h");

    addSystemIncludeFile(&file, "sstream");
    addProjectIncludeFile(&file, "Base/Core/Common/include/Hash.h");
    addProjectIncludeFile(&file, "Base/Logger/include/Logger.h");
    addBlankLine(&file);
    supressMsvcWarning(&file, 4307);
    addBlankLine(&file);

    openNamespace(&file, "ssbl");
    openNamespace(&file,
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetSkeletonName());
    file << "#define SSBL_LONG_MIN  (-2147483647L - 1)" << endl;
    file << "#define SSBL_LLONG_MIN  (-9223372036854775807i64 - 1)" << endl;

    addBlankLine(&file);
    vector<CoLaVariable*> vars =
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetVariables();
    for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
         ++it) {
      writeVariableImplementations(&file, (*it));
    }

    closeNamespace(
        &file,
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
    closeNamespace(&file, "ssbl");
    addBlankLine(&file);
    file.close();
  }
}



//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableImplementations(
    ofstream* pSourceFile,
                                                CoLaVariable * pVar) {
    addBlankLine(pSourceFile);
  *pSourceFile << pVar->getName() + "_" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                            ->GetDeviceName() +
                      "_Var::";
    *pSourceFile << pVar->getName() + "_" +
                        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                        "_Var()"
                 << std::endl;

    *pSourceFile << "\t: " +
                        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                            ->GetDeviceName() +
                        "_Var(\"";
    *pSourceFile << pVar->getName() << "\",\"" << pVar->getComName() << "\","
                 << pVar->getVariableIndex() << ",";

    if (pVar->isReadOnly()) {
      *pSourceFile << "READ_ONLY,";
    } else {
      *pSourceFile << "READ_WRITE,";
    }
    *pSourceFile << pVar->getViewAccessLvl() << ",";

    if (!pVar->isReadOnly()) {
      *pSourceFile << pVar->getWriteAccessLvl() << ",";
    } else {
      *pSourceFile << "LEVEL_INVALID,";
    }
    if (!pVar->providesEvent()) {
      *pSourceFile << "-1";
    } else {
      *pSourceFile << pVar->getEventIndex();
    }
    *pSourceFile << ")" << endl;

    *pSourceFile << "{" << std::endl;

    *pSourceFile << "}" << std::endl;
    addBlankLine(pSourceFile);

    writeVariableGetterSetterImplementations(pSourceFile, pVar);
  }

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableGetterSetterImplementations(
  std::ofstream* pSourceFile, CoLaVariable* pVar) {
  std::vector<tuple<std::string, std::string, std::string, std::string>>
      elems;
  findElements(pVar->getType(), "", elems, 0, 0, false);

  for (auto s : BaseTypes_) {
    vector<string> bts;
    vector<string> kts;
    for (auto x : elems) {
      string type = std::get<0>(x);
      string name = std::get<1>(x);
      if (((0 == type.compare(s)) && (0 != name.size())) ||
          ((0 == s.compare("uint8_t")) && (0 == type.compare("Enum8_t")) &&
            (0 != name.size())) ||
          ((0 == s.compare("uint16_t")) && (0 == type.compare("Enum16_t")) &&
            (0 != name.size()))) {
        bts.push_back(std::get<1>(x));
      } else if ((0 == type.compare(s)) ||
                  ((0 == s.compare("uint8_t")) &&
                  (0 == type.compare("Enum8_t"))) ||
                  ((0 == s.compare("uint16_t")) &&
                  (0 == type.compare("Enum16_t")))) {
        kts.push_back(std::get<1>(x));
      }
    }

    writeVariableGetterImplementations(pSourceFile, pVar, s, bts);
    writeSimpleVariableGetterImplementations(pSourceFile, pVar, s, kts);
  }
  bool simpleType = false;
  string simpleTypeName = "";
  for (auto s : BaseTypes_) {
    vector<tuple<std::string, std::string, std::string>> bts;
    vector<tuple<std::string, std::string, std::string>> kts;
    for (auto x : elems) {
      string type = std::get<0>(x);
      string name = std::get<1>(x);
      if (((0 == type.compare(s)) && (0 != name.size())) ||
          ((0 == s.compare("uint8_t")) && (0 == type.compare("Enum8_t")) &&
            (0 != name.size())) ||
          ((0 == s.compare("uint16_t")) && (0 == type.compare("Enum16_t")) &&
            (0 != name.size()))) {
        tuple<std::string, std::string, std::string> k = {
            std::get<1>(x), std::get<2>(x), std::get<3>(x)};
        bts.push_back(k);
      } else if ((0 == type.compare(s)) ||
                  ((0 == s.compare("uint8_t")) &&
                  (0 == type.compare("Enum8_t"))) ||
                  ((0 == s.compare("uint16_t")) &&
                  (0 == type.compare("Enum16_t")))) {
        tuple<std::string, std::string, std::string> k = {s, std::get<2>(x),
                                                          std::get<3>(x)};
        simpleTypeName = s;
        kts.push_back(k);
        simpleType = true;
      }
    }

    writeVariableSetterImplementations(pSourceFile, pVar, s, bts);
    writeSimpleVariableSetterImplementations(pSourceFile, pVar, s, kts);
  }

  *pSourceFile << "SensorResult ";

  *pSourceFile
      << pVar->getName() + "_" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Var::SetBasicFromString(std::string value)"
                << endl;
  *pSourceFile << "{" << endl;
  *pSourceFile << "\tSensorResult ret = SSBL_ERR_VARIABLE_ELEMENT_NOT_FOUND;"
                << endl;
  if (simpleType == false) {
    *pSourceFile << "\tSSBL_UNUSED(value);" << endl;
    *pSourceFile << "\tSSBL_LOG_WARNING(\"Variable is not a simple type\");"
                  << endl;
  } else {
    if (0 == simpleTypeName.compare("uint8_t") ||
        0 == simpleTypeName.compare("int8_t")) {
      *pSourceFile << "\tbool isNegative = false;" << endl;
      *pSourceFile << "\tstd::string help = value;" << endl;
      *pSourceFile << "\tif ('+' == value[0]) {" << endl;
      *pSourceFile << "\t\thelp = value.substr(1);" << endl;
      *pSourceFile << "\t} else if ('-' == value[0]) { " << endl;
      *pSourceFile << "\t\tisNegative = true; " << endl;
      *pSourceFile << "\t\thelp = value.substr(1); " << endl;
      *pSourceFile << "\t} else { " << endl;
      *pSourceFile << "\t\thelp = value;" << endl;
      *pSourceFile << "\t}" << endl;

      *pSourceFile << "\tint8_t helpInt = (int8_t)std::stoi(help);" << endl;
      *pSourceFile << "\tif (isNegative)" << endl;
      *pSourceFile << "\t\thelpInt *= -1;" << endl;

      *pSourceFile << "\tmemcpy(&this->Value_, &helpInt,1);" << endl;

    } else {
      *pSourceFile << "\tstd::stringstream s(value);" << endl;
      *pSourceFile << "\ts>> this->Value_;" << endl;
    }
    *pSourceFile << "\tret = SSBL_SUCCESS;" << endl;
  }
  *pSourceFile << "\treturn ret;" << endl;
  *pSourceFile << "}" << endl;
}


//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableGetterImplementations(
    std::ofstream* pSourceFile,
    CoLaVariable* pVar,
    const string& baseTypeName,
    vector<string> addrs) {
  *pSourceFile << "SensorResult ";
  *pSourceFile
      << pVar->getName() + "_" +
             static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Var::";
  *pSourceFile << "GetBasicElement(const std::string& elementName, " +
                      baseTypeName + "& value) "
               << endl;
  *pSourceFile << "{" << endl;
  *pSourceFile << "\tSensorResult ret = SSBL_ERR_VARIABLE_ELEMENT_NOT_FOUND;"
               << endl;

  if (0 == addrs.size()) {
    *pSourceFile << "\tSSBL_UNUSED(elementName);" << endl;
    *pSourceFile << "\tSSBL_UNUSED(value);" << endl;
    *pSourceFile << "\tSSBL_LOG_WARNING(\"Variable does either not contain "
                    "element %s of type " +
                        baseTypeName +
                        ", or the address string does not yield an unique "
                        "element.\", elementName.c_str());"
                 << endl;
  }
  if (0 != addrs.size()) {
    *pSourceFile << "\tuint64_t test = hash_64_fnv1a(";
    *pSourceFile << "elementName.c_str(), elementName.size());" << endl;

    *pSourceFile << "\tswitch (test)" << endl;
    *pSourceFile << "\t{" << endl;
    vector<string> whitelist;
    for (auto x : addrs) {
      whitelist.push_back(x.substr(x.rfind(".") + 1));
    }

    unique_strings(whitelist);

    for (auto x : addrs) {
      if (string::npos == x.find(".")) {
        WriteVariableGetterCaseImplementation(pSourceFile, x, x);
      } else {
        string last = x.substr(x.rfind(".") + 1);
        if (checkIfStringIsUniqueInVector(last, whitelist)) {
          WriteVariableGetterCaseImplementation(pSourceFile, last, x);
        }
        WriteVariableGetterCaseImplementation(pSourceFile, x, x);
      }
    }
    *pSourceFile << "\t\tdefault:" << endl;
    *pSourceFile << "\t\t\tSSBL_LOG_WARNING(\"Variable does either not contain "
                    "element %s of type " +
                        baseTypeName +
                        ", or the address string does not yield an unique "
                        "element.\", elementName.c_str());"
                 << endl;
    *pSourceFile << "\t\t\tret = SSBL_ERR_VARIABLE_ELEMENT_NOT_FOUND;" << endl;
    *pSourceFile << "\t\t\tbreak;" << endl;
    *pSourceFile << "\t}" << endl;
  }
  *pSourceFile << "\treturn ret;" << endl;
  *pSourceFile << "}" << endl;
  addBlankLine(pSourceFile);
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableSetterImplementations(
    std::ofstream* pSourceFile, CoLaVariable* pVar, const string& baseTypeName,
    vector<tuple<std::string, std::string, std::string>> addrs) {
  *pSourceFile << "SensorResult ";
  *pSourceFile
      << pVar->getName() + "_" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Var::";
  *pSourceFile << "SetBasicElement(const std::string& elementName, " +
                      baseTypeName + "& value) "
               << endl;
  *pSourceFile << "{" << endl;
  *pSourceFile << "\tSensorResult ret = SSBL_ERR_VARIABLE_ELEMENT_NOT_FOUND;"
               << endl;

  if (0 == addrs.size()) {
    *pSourceFile << "\tSSBL_UNUSED(elementName);" << endl;
    *pSourceFile << "\tSSBL_UNUSED(value);" << endl;
    *pSourceFile << "\tSSBL_LOG_WARNING(\"Variable does either not contain "
                    "element %s of type " +
                        baseTypeName +
                        ", or the address string does not yield an unique "
                        "element.\", elementName.c_str());"
                 << endl;
  }
  if (0 != addrs.size()) {
    *pSourceFile << "\tuint64_t test = hash_64_fnv1a(";
    *pSourceFile << "elementName.c_str(), elementName.size());" << endl;

    *pSourceFile << "\tswitch (test)" << endl;
    *pSourceFile << "\t{" << endl;
    vector<string> whitelist;
    for (auto x : addrs) {
      string out = std::get<0>(x);

      whitelist.push_back(out.substr(out.rfind(".") + 1));
    }

    unique_strings(whitelist);

    for (auto x : addrs) {
      string out = std::get<0>(x);
      if (string::npos == out.find(".")) {
        WriteVariableSetterCaseImplementation(pSourceFile, out, out,
                                              std::get<1>(x), std::get<2>(x));
      } else {
        string last = out.substr(out.rfind(".") + 1);
        if (checkIfStringIsUniqueInVector(last, whitelist)) {
          WriteVariableSetterCaseImplementation(pSourceFile, last, out,
                                                std::get<1>(x), std::get<2>(x));
        }
        WriteVariableSetterCaseImplementation(pSourceFile, out, out,
                                              std::get<1>(x), std::get<2>(x));
      }
    }
    *pSourceFile << "\t\tdefault:" << endl;
    *pSourceFile << "\t\t\tSSBL_LOG_WARNING(\"Variable does either not contain "
                    "element %s of type " +
                        baseTypeName +
                        ", or the address string does not yield an unique "
                        "element.\", elementName.c_str());"
                 << endl;
    *pSourceFile << "\t\t\tret = SSBL_ERR_VARIABLE_ELEMENT_NOT_FOUND;" << endl;
    *pSourceFile << "\t\t\tbreak;" << endl;
    *pSourceFile << "\t}" << endl;
  }
  *pSourceFile << "\treturn ret;" << endl;
  *pSourceFile << "}" << endl;
  addBlankLine(pSourceFile);
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeSimpleVariableGetterImplementations(
    std::ofstream* pSourceFile, CoLaVariable* pVar, const string& baseTypeName,
    vector<string> addrs) {
    *pSourceFile << "SensorResult ";
  *pSourceFile
      << pVar->getName() + "_" +
                        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                            ->GetDeviceName() +
                        "_Var::";
    *pSourceFile << "GetBasic(" + baseTypeName + "& value) " << endl;
    *pSourceFile << "{" << endl;
    *pSourceFile << "\tSensorResult ret = SSBL_ERR_VARIABLE_ELEMENT_NOT_FOUND;"
                 << endl;

    if (0 == addrs.size()) {
      *pSourceFile << "\tSSBL_UNUSED(value);" << endl;
      *pSourceFile << "\tSSBL_LOG_WARNING(\"Variable not of type " +
                          baseTypeName + "\");"
                   << endl;
    }

    if (0 != addrs.size()) {
      *pSourceFile << "\tret = SSBL_SUCCESS;" << endl;
      *pSourceFile << "\tvalue = this->Value_;" << endl;
    }
    *pSourceFile << "\treturn ret;" << endl;
    *pSourceFile << "}" << endl;
    addBlankLine(pSourceFile);
}


//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeSimpleVariableSetterImplementations(
        std::ofstream * pSourceFile, CoLaVariable* pVar,
        const string& baseTypeName,
        vector<tuple<std::string, std::string, std::string>> addrs) {
  *pSourceFile << "SensorResult ";
  *pSourceFile
      << pVar->getName() + "_" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Var::";
  *pSourceFile << "SetBasic(" + baseTypeName + "& value) " << endl;
  *pSourceFile << "{" << endl;
  *pSourceFile << "\tSensorResult ret = SSBL_ERR_VARIABLE_ELEMENT_NOT_FOUND;"
                << endl;

  if (0 == addrs.size()) {
    *pSourceFile << "\tSSBL_UNUSED(value);" << endl;
    *pSourceFile << "\tSSBL_LOG_WARNING(\"Variable is not a simple type of " +
                        baseTypeName + "\");"
                  << endl;
  }

  if (0 != addrs.size()) {
    string type = std::get<0>(addrs[0]);
    string min = std::get<1>(addrs[0]);
    string max = std::get<2>(addrs[0]);

    if (((0 == min.compare("UNSET")) && (0 == max.compare("UNSET"))) ||
        (0 == min.size() && 0 == max.size())) {
      *pSourceFile << "\tthis->Value_ = value;" << endl;
      *pSourceFile << "\tret = SSBL_SUCCESS;" << endl;
    } else {
      *pSourceFile << "\tif((value >= " + min + ") &&  (value <= " + max + ")){"
                    << endl;
      *pSourceFile << "\t\tthis->Value_ = value;" << endl;
      *pSourceFile << "\t\tret = SSBL_SUCCESS;" << endl;
      *pSourceFile << "\t}" << endl;
    }
  }
  *pSourceFile << "\treturn ret;" << endl;
  *pSourceFile << "}" << endl;
  addBlankLine(pSourceFile);
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::WriteVariableSetterCaseImplementation(
  std::ofstream* pSourceFile, const string& caseName, const string& element,
  const string& min, const string& max) {
  *pSourceFile << "\t\tcase hash_64_fnv1a_const(\"" + caseName + "\"):" << endl;

  if (((0 == min.compare("UNSET")) && (0 == max.compare("UNSET"))) ||
      (0 == min.size() && 0 == max.size())) {
    *pSourceFile << "\t\t\tthis->Value_." + element + " = value;" << endl;
    *pSourceFile << "\t\t\tret = SSBL_SUCCESS;" << endl;
  } else {
    *pSourceFile << "\t\t\tif((value >= " + min + ") &&  (value <= " + max +
                        ")){"
                 << endl;
    *pSourceFile << "\t\t\t\tthis->Value_." + element + " = value;" << endl;
    *pSourceFile << "\t\t\t\tret = SSBL_SUCCESS;" << endl;
    *pSourceFile << "\t\t\t} else {" << endl;
    *pSourceFile << "\t\t\t\tSSBL_LOG_WARNING(\"Value is out of range. Min: ";
    *pSourceFile << min + " Max: " + max + "\");" << endl;
    *pSourceFile << "\t\t\t\tret = SSBL_ERR_VALUE_OUT_OF_RANGE;" << endl;
    *pSourceFile << "\t\t\t}" << endl;
  }
  *pSourceFile << "\t\t\tbreak;" << endl;
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::WriteVariableGetterCaseImplementation(
    std::ofstream* pSourceFile, const string& caseName, const string& element) {
  *pSourceFile << "\t\tcase hash_64_fnv1a_const(\"" + caseName + "\"):" << endl;
  *pSourceFile << "\t\t\tvalue = this->Value_." + element + ";" << endl;
  *pSourceFile << "\t\t\tret = SSBL_SUCCESS;" << endl;
  *pSourceFile << "\t\t\tbreak;" << endl;
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::findElements(
  GenericCoLaTypeContainer* pType, 
  const string& prefix,
  std::vector<tuple<std::string, std::string, std::string, std::string>>& addresses,
    uint32_t lvl, uint32_t arrayDepth, bool lastArray) 
{
  string iPrefix = prefix;

  if (0 == pType->GetLocalName().compare("xbState")) {
    cout << "stop" << endl;
  }
  switch (pType->GetTypeClass()) {
    case STRUCT_CONTAINER: {
      vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();
      if (0 != iPrefix.size() && (lastArray == false)) {
        char ch = iPrefix.back();
        if (ch == '.') {
          iPrefix = iPrefix + pType->GetLocalName();
        } else {
          iPrefix = iPrefix + "." + pType->GetLocalName();
        }

      } else if (0 != lvl && (lastArray == false)) {
        if (0 != iPrefix.size()) {
          iPrefix = iPrefix + "." + pType->GetLocalName();
        } else {
          iPrefix = pType->GetLocalName();
        }
      }
      for (auto x : subTypeList) {
        findElements(x, iPrefix, addresses, lvl + 1, arrayDepth, false);
      }
      break;
    }

    case USER_TYPE_CONTAINER: {
      GenericCoLaTypeContainer* pL;
      pL = static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
               ->GetTypeByBaseName(pType->GetBaseTypeName());

      if (pL->GetTypeClass() == STRUCT_CONTAINER) {
        pL->SetLocalName(pType->GetLocalName());
      }

      cout << "User type: " << iPrefix << endl;
      findElements(pL, iPrefix, addresses, lvl, arrayDepth, lastArray);
      break;
    }
    case ARRAY_CONTAINER: {
      ArrayContainer* pA = dynamic_cast<ArrayContainer*>(pType);
      vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();

      if (pA->IsFlexArray()) {
        string name = pType->GetLocalName();
        // uiLengthScanData_aEncoderBlock
        if (iPrefix.size() == 0) {
          name = "uiLength" + pType->GetLocalName();
        } else {
          char ch = iPrefix.back();
          if (ch == '.') {
            name = iPrefix + "uiLength" + pType->GetLocalName();
          } else {
            name = iPrefix + ".uiLength" + pType->GetLocalName();
          }
        }

        cout << "Basic type: " << name << endl;
        tuple<std::string, std::string, std::string, std::string> v = {
            string("uint16_t"), name, to_string(pA->GetArraySize()),
            to_string(pA->GetArraySize())};
        addresses.push_back(v);
      }

      if (0 != iPrefix.size()) {
        iPrefix = iPrefix + "." + pType->GetLocalName() + "[";
      } else {
        iPrefix = iPrefix + pType->GetLocalName() + "[";
      }

      for (uint32_t i = 0; i < pA->GetArraySize(); i++) {
        string tmp = iPrefix + to_string(i) + "]";

        cout << "Array type: " << tmp << endl;
        if ((subTypeList[0]->GetTypeClass() != CONCRETE_TYPE_CONTAINER) &&
            (subTypeList[0]->GetTypeClass() != ENUM_CONTAINER)) {
          findElements(subTypeList[0], tmp, addresses, lvl + 1, arrayDepth++,
                       true);
        }
      }

      break;
    }
    case ENUM_CONTAINER:
    case CONCRETE_TYPE_CONTAINER: {
      if (0 != iPrefix.size()) {
        if (lastArray == false) {
          iPrefix = iPrefix + "." + pType->GetLocalName();
        }
      } else {
        if (lvl != 0) {
          iPrefix = pType->GetLocalName();
        } else {
        }
      }
      cout << "Basic type: " << iPrefix << endl;

      tuple<std::string, std::string, std::string, std::string> v = {
          pType->GetBaseTypeName(), iPrefix, pType->GetMin(), pType->GetMax()};

      addresses.push_back(v);

      break;
    }
    case STRING_CONTAINER:
      break;
    case XBYTE_CONTAINER:
      break;
  }
  return;
}