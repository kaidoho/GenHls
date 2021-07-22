#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include "Writer/include/Writer.h"

using namespace std;

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
string arrayDepthToCounter(int32_t arrayDepth) {
  std::string Counter;
  for (int32_t i = 0; i <= arrayDepth; i++) {
    Counter.append("k");
  }

  return Counter;
}
//===========================================================================
string getIndent(int32_t arrayDepth) {
  std::string Indent = "\t";
  for (int32_t i = 0; i <= arrayDepth; i++) {
    Indent.append("\t");
  }
  return Indent;
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
void Writer::writeDeviceClassImplementation(std::string& folder) {
  ofstream file;
  string FileString = folder + "/" + pDev_->GetSkeletonName() + ".cpp";

  file.open(FileString.c_str(), ios::out | ios::trunc);

  if (file.is_open()) {
    writeNoticeHeader(&file);
    addBlankLine(&file);
    addLocalIncludeFile(&file, ".h");
    addLocalIncludeFile(&file, "_CoLa_Extension.h");
    addProjectIncludeFile(&file, "Base/Core/Common/include/Assert.h");
    addProjectIncludeFile(&file, "Base/Core/Common/include/MakeUnique.h");
    addProjectIncludeFile(&file, "Base/Core/OS/include/Time.h");
    addBlankLine(&file);

    openNamespace(&file, "ssbl");
    openNamespace(&file, "Dev" + pDev_->GetSkeletonName());
    addBlankLine(&file);

    file << "//TODO: Buffer size is set fixed" << endl;
    file << "//TODO: default interface" << endl;
    file << pDev_->GetSkeletonName() + "::" + pDev_->GetSkeletonName();
    file << "(const std::string& Ip, const std::string& interfaceName,const "
            "std::string& localName)"
         << endl;
    file << "\t: CoLaSensorSkeleton(localName,";
    file << "16 * 1024,16 * 1024";

    std::vector<CoLaUserLevel*> levels = pDev_->GetUserLevels();
    file << ")" << endl;
    file << "{" << endl;

    file << "\tSensorName_=\"" + pDev_->GetSkeletonName() + "\";" << endl;

    file << "\tpasswords_[LEVEL_INVALID] = 0;" << endl;

    for (vector<CoLaUserLevel*>::iterator it = levels.begin();
         it != levels.end(); ++it) {
      file << "\tpasswords_[" << (*it)->getUserLvlName();
      file << "] = " << (*it)->getUserLvlPassword() << ";" << endl;
    }

    file << "\tUserSelectedIpOrSerial_ = Ip;" << endl;
    file << "\tUserSelectedInterfaceName_ = interfaceName;" << endl;

    std::vector<SensorInterface*> iFaces = pDev_->GetInterfaces();

    for (vector<SensorInterface*>::iterator it = iFaces.begin();
         it != iFaces.end(); ++it) {
      std::vector<SensorInterfaceProtocol*> iFaceProtos = (*it)->GetProtocols();
      for (vector<SensorInterfaceProtocol*>::iterator iit = iFaceProtos.begin();
           iit != iFaceProtos.end(); ++iit) {
        file << "\tAvailableSensorInterfaces_.push_back(ssbl::make_unique<"
                "SickSensorInterfaceDescription*>( new "
                "SickSensorInterfaceDescription(";
        file << "\"" + (*it)->GetName() + "\"" + " , ";
        file << (*it)->GetInterfaceType() + " , ";
        file << (*iit)->GetProtocolType() + " , ";
        file << (*iit)->GetAddressing() + " , ";
        file << std::to_string((*it)->GetPort()) + " , " << endl;
        if ((*iit)->IsProtocolSwitchPossible()) {
          file << "\t\t{";

          for (vector<SensorInterfaceProtocol*>::iterator k =
                   iFaceProtos.begin();
               k != iFaceProtos.end(); ++k) {
            if (k != iFaceProtos.begin()) {
              file << ",";
            }
            file << "std::make_tuple(" + (*k)->GetProtocolType() + ", \"" +
                        (*k)->GetSwitchVar() + "\", \"" +
                        (*k)->GetSwitchVarValue() + "\")";
          }

          file << "\t\t})));" << endl;
        } else {
          file << "\t\t{})));" << endl;
        }
        /*
          new SickSensorInterfaceDescription("CoLaA Port", ETHERNET_INTERFACE,
                                             COLA_A, BY_NAME, 2111,
                                             {std::make_tuple(COLA_A, "x" ,"x")}
                          */
      }
    }
    vector<CoLaVariable*> vars = pDev_->GeVariables();
    for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
         ++it) {
      file << "\tVariableRepo.RegisterComObj(\"" + pDev_->GetSkeletonName() +
                  (*it)->getName() + "\",";
      file << (*it)->getName() + "_" + pDev_->GetSkeletonName() +
                  "_Var::Create); "
           << endl;
    }
    vector<CoLaMethod*> mths = pDev_->GetMethods();
    for (vector<CoLaMethod*>::iterator it = mths.begin(); it != mths.end();
         ++it) {
      file << "\tFunctionRepo.RegisterComObj(\"" + pDev_->GetSkeletonName() +
                  (*it)->getName() + "\",";
      file << (*it)->getName() + "_" + pDev_->GetSkeletonName() +
                  "_Func::Create); "
           << endl;
    }

    file << "}" << endl;
    addBlankLine(&file);
    file << pDev_->GetSkeletonName() + "::~" + pDev_->GetSkeletonName() + "()"
         << endl;
    file << "{" << endl;
    file << "\tfor (auto& x : AvailableSensorInterfaces_) {" << endl;
    file << "\t\tSickSensorInterfaceDescription* p = *(x.release());" << endl;
    file << "\t\tdelete p;" << endl;
    file << "\t}" << endl;
    file << "\tAvailableSensorInterfaces_.clear();" << endl;
    file << "}" << endl;

    addBlankLine(&file);
    file << "Protocol * " + pDev_->GetSkeletonName() +
                "::DeviceSpecificProtocolInit(ProtocolType Protocol, "
                "AddressingMode AddrMode)"
         << endl;
    file << "{" << endl;
    file << "\tssbl::Protocol * pRet = NULL;" << endl;
    file << "\tswitch (Protocol)" << endl;
    file << "\t{" << endl;

    vector<std::string> done;

    std::vector<SensorInterfaceProtocol*> iFaceProtos = pDev_->GetProtocols();

    for (vector<SensorInterfaceProtocol*>::iterator it = iFaceProtos.begin();
         it != iFaceProtos.end(); ++it) {
      if (std::find(done.begin(), done.end(), (*it)->GetProtocolType()) ==
          done.end()) {
        file << "\tcase " << (*it)->GetProtocolType() + ":" << endl;
        file << "\t\tpRet = new " + pDev_->GetSkeletonName() + "_";

        if (0 == ((*it)->GetProtocolType()).compare("COLA_A")) {
          file << "CoLaA";
        }
        if (0 == ((*it)->GetProtocolType()).compare("COLA_B")) {
          file << "CoLaB";
        }
        if (0 == ((*it)->GetProtocolType()).compare("COLA_2")) {
          file << "CoLa2";
        }
        file << "Protocol(AddrMode, max_tx_buffer_size_, "
                "max_rx_buffer_size_);"
             << endl;

        //      file << "\t\tret = SSBL_SUCCESS;" << endl;
        file << "\t\tbreak;" << endl;
        done.push_back((*it)->GetProtocolType());
      }
    }

    file << "\tdefault:" << endl;
    file << "\t\tSSBL_LOG_ERROR(\"Unknown protocol\");" << endl;
    file << "\t\tbreak;" << endl;
    file << "\t}" << endl;
    addBlankLine(&file);
    file << "\treturn pRet;" << endl;
    file << "}" << endl;

    addBlankLine(&file);
    file << "SensorResult " + pDev_->GetSkeletonName() +
                "::DeviceSpecificProtocolSwitch(ProtocolType Protocol)"
         << endl;
    file << "{" << endl;
    file << "\tSensorResult ret = SSBL_ERR_PROTOCOL_NOT_DETECTED;" << endl;

    file << "\tif(pActiveInterface_->ProtocolCanBeSwitched()){" << endl;
    file << "\t\tstd::vector<std::tuple<ProtocolType, std::string, "
            "std::string>> info = "
            "pActiveInterface_->GetInterfaceDescription()->GetSwitchInfo();"
         << endl;
    addBlankLine(&file);
    file << "\t\tfor (auto x : info) {" << endl;
    file << "\t\t\tif (std::get<0>(x) == Protocol) {" << endl;
    file << "\t\t\t\tauto v = this->CreateVariable(std::get<1>(x));" << endl;
    file << "\t\t\t\tv->SetBasicFromString(std::get<2>(x));" << endl;
    file << "\t\t\t\tret = CoLaSensorSkeleton::WriteVariable(*v);" << endl;

    file << "\t\t\t\tif(SSBL_SUCCESS == ret)" << endl;
    file << "\t\t\t\t{" << endl;
    file << "\t\t\t\t\t ret = StoreParameter();" << endl;

    file << "\t\t\t\t\tif(SSBL_SUCCESS == ret)" << endl;
    file << "\t\t\t\t\t{" << endl;

    file << "\t\t\t\t\t\t ret = RebootSensor();" << endl;

    file << "\t\t\t\t\t} else {" << endl;
    file << "\t\t\t\t\t\tSSBL_LOG_ERROR(\"Can't store parameter\");" << endl;
    file << "\t\t\t\t\t}" << endl;
    file << "\t\t\t\t}" << endl;
    file << "\t\t\t\tdelete v;" << endl;
    file << "\t\t\t\tbreak;" << endl;
    file << "\t\t\t}" << endl;
    file << "\t\t}" << endl;

    addBlankLine(&file);
    file << "\t}" << endl;
    file << "\treturn ret;" << endl;
    file << "}" << endl;
    addBlankLine(&file);

    file << "SensorResult " + pDev_->GetSkeletonName() + "::StoreParameter()"
         << endl;
    file << "{" << endl;
    file << "\tSensorResult ret = SSBL_ERR_FEATURE_NOT_SUPPORTED;" << endl;

    if (pDev_->StoreParamGiven()) {
      file << "\tauto m = CreateFunction(\"" + pDev_->GetStoreParamFuncName() +
                  "\");"
           << endl;
      file << "\tret = CoLaSensorSkeleton::CallFunction(*m);" << endl;
      file << "\tSSBL_Sleep(" << to_string(pDev_->GetStoreParamFuncTime())
           << ");" << endl;
      file << "\tdelete m;" << endl;
    }
    file << "\treturn ret;" << endl;
    file << "}" << endl;
    addBlankLine(&file);

    file << "SensorResult " + pDev_->GetSkeletonName() + "::RebootSensor()"
         << endl;
    file << "{" << endl;
    file << "\tSensorResult ret = SSBL_ERR_FEATURE_NOT_SUPPORTED;" << endl;

    if (pDev_->RebootGiven()) {
      file << "\tauto m = CreateFunction(\"" + pDev_->GetRebootFuncName() +
                  "\");"
           << endl;
      file << "\tret = CoLaSensorSkeleton::CallFunction(*m);" << endl;

      file << "\tSSBL_Sleep(" << to_string(pDev_->GetRebootFuncTime()) << ");"
           << endl;

      file << "\tdelete m;" << endl;
    }
    file << "\treturn ret;" << endl;
    file << "}" << endl;

    closeNamespace(&file, "Dev" + pDev_->GetSkeletonName());
    closeNamespace(&file, "ssbl");
    file.close();
  }
}

//===========================================================================
//===========================================================================
void Writer::writeVarSerializeMethod(ofstream* pHeaderFile,
                                     GenericCoLaTypeContainer* pType,
                                     std::string& toplevelName,
                                     int32_t arrayDepth, bool lastArray,
                                     std::string coladialect) {
  string CounterName = arrayDepthToCounter(arrayDepth);
  string indent = getIndent(arrayDepth);

  TypeClass tc = pType->GetTypeClass();

  switch (tc) {
    case STRUCT_CONTAINER: {
      vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();
      *pHeaderFile << endl;

      for (vector<GenericCoLaTypeContainer*>::iterator it =
               subTypeList.begin();
           it != subTypeList.end(); ++it) {
        lastArray = false;
        writeVarSerializeMethod(pHeaderFile, (*it), toplevelName, arrayDepth,
                                lastArray, coladialect);
      }
      break;
    }
    case XBYTE_CONTAINER: {
      string prefix = coladialect + "Serializer::";
      *pHeaderFile << indent << "for(int32_t " << CounterName << "=0;"
                   << CounterName << "<"
                   << (dynamic_cast<XByteContainer*>(pType))->GetWidthInBytes()
                   << ";++" << CounterName << ")" << endl;
      *pHeaderFile << indent << "{" << endl;
      *pHeaderFile << indent << "\t" << prefix + "Serialize(pDest, rSrc."
                   << toplevelName << pType->GetLocalName()
                   << "[" + CounterName + "],";
      *pHeaderFile << "pOffset);" << endl;
      *pHeaderFile << indent << "}" << endl;
      break;
    }
    case ARRAY_CONTAINER: {
      arrayDepth++;
      std::vector<GenericCoLaTypeContainer*> subTypeList =
          pType->GetChildren();

      if ((dynamic_cast<ArrayContainer*>(pType))->IsFlexArray()) {
        *pHeaderFile << indent
                     << coladialect +
                            "Serializer::Serialize(pDest, rSrc.uiLength" +
                            pType->GetLocalName() + ", pOffset);"
                     << endl;
        *pHeaderFile << indent << "for(int32_t " << CounterName << "=0;"
                     << CounterName << "<"
                     << "rSrc.uiLength" + pType->GetLocalName() << ";++"
                     << CounterName << ")" << endl;
      } else {
        *pHeaderFile << indent << "for(int32_t " << CounterName << "=0;"
                     << CounterName << "<"
                     << (dynamic_cast<ArrayContainer*>(pType))->GetArraySize()
                     << ";++" << CounterName << ")" << endl;
      }

      *pHeaderFile << indent << "{" << endl;
      for (vector<GenericCoLaTypeContainer*>::iterator it =
               subTypeList.begin();
           it != subTypeList.end(); ++it) {
        lastArray = true;
        writeVarSerializeMethod(
            pHeaderFile, (*it),
            toplevelName + pType->GetLocalName() + "[" + CounterName + "].",
            arrayDepth, lastArray, coladialect);
      }
      arrayDepth--;
      *pHeaderFile << indent << "}" << endl;

      break;
    }
    case ENUM_CONTAINER:
    case CONCRETE_TYPE_CONTAINER: {
      string prefix = coladialect + "Serializer::";

      if (!lastArray) {
        *pHeaderFile << indent << prefix + "Serialize(pDest, rSrc."
                     << toplevelName << pType->GetLocalName() << " , pOffset);"
                     << endl;
      } else {
        CounterName = arrayDepthToCounter(arrayDepth - 1);
        toplevelName.pop_back();
        *pHeaderFile << indent << prefix + "Serialize(pDest, rSrc."
                     << toplevelName << ", pOffset);" << endl;
      }
      break;
    }
    case STRING_CONTAINER: {
      StringContainer* pS = dynamic_cast<StringContainer*>(pType);
      if (pS->isFixLength()) {
        *pHeaderFile << indent
                     << "SerializeFixString(pDest, rSrc.Text," +
                            to_string(pS->GetLength()) + ", pOffset);"
                     << endl;
      } else {
        *pHeaderFile << indent
                     << "SerializeFlexString(pDest, rSrc.Text, rSrc.uiLength," +
                            to_string(pS->GetLength()) + ", pOffset);"
                     << endl;
      }

      break;
    }
    case USER_TYPE_CONTAINER: {
      if (!lastArray) {
        *pHeaderFile << indent << "Serialize(pDest, rSrc." << toplevelName
                     << pType->GetLocalName() << " , pOffset);" << endl;
      } else {
        CounterName = arrayDepthToCounter(arrayDepth - 1);
        toplevelName.pop_back();
        *pHeaderFile << indent << "Serialize(pDest, rSrc." << toplevelName
                     << ", pOffset);" << endl;
      }
      break;
    }
    default:
      cout << "UPS" << endl;
      break;
  }
  // string CounterName = arrayDepthToCounter(arrayDepth);
  // string indent = getIndent(arrayDepth);
  //
  // TypeClass tc=  pType->GetTypeClass();

  // switch(tc)
  //{

  //  case ARRAY_CONTAINER:
  //  {
  //    arrayDepth++;
  //    std::vector<GenericCoLaTypeContainer*> subTypeList =
  //    pType->GetChildren();

  //    if ((dynamic_cast<ArrayContainer*> (pType))->IsFlexArray())
  //    {
  //      *pHeaderFile << indent << coladialect + "Serializer::Serialize(pDest,
  //      rSrc.length, pOffset);" << endl; *pHeaderFile << indent <<
  //      "for(int32_t " << CounterName << "=0;" << CounterName << "<" <<
  //      "rSrc.length" << ";++" << CounterName << ")" << endl;
  //    }
  //    else
  //    {
  //      *pHeaderFile << indent << "for(int32_t " << CounterName << "=0;" <<
  //      CounterName << "<" << (dynamic_cast<ArrayContainer*>
  //      (pType))->GetArraySize() << ";++" << CounterName << ")" << endl;
  //    }

  //    *pHeaderFile << indent << "{" << endl;
  //    for (vector<GenericCoLaTypeContainer*>::iterator it =
  //    subTypeList.begin(); it != subTypeList.end(); ++it)
  //    {
  //      lastArray = true;
  //      writeVarSerializeMethod(pHeaderFile, (*it), toplevelName +
  //      pType->GetLocalName() + "[" + CounterName + "].", arrayDepth,
  //      lastArray, coladialect);
  //    }
  //    arrayDepth--;
  //    *pHeaderFile << indent << "}" << endl;
  //    break;
  //  }
  //  case XBYTE_CONTAINER:
  //  {
  //    *pHeaderFile << indent << "for (int32_t xb=0; xb < sizeof(rSrc." +
  //    pType->GetLocalName() + ");++xb)" << endl; *pHeaderFile << indent << "{"
  //    << endl; *pHeaderFile << indent << "\t" << coladialect +
  //    "Serializer::Serialize(pDest, rSrc." + pType->GetLocalName() + "[xb],
  //    pOffset);" << endl; *pHeaderFile << indent << "}" << endl;

  //  }
  //  case CONCRETE_TYPE_CONTAINER:
  //  {

  //    if (pType->GetUserType())
  //    {
  //      for (vector<GenericSopasType*>::iterator it = sopasTypes_.begin(); it
  //      != sopasTypes_.end(); ++it)
  //      {
  //        if (0 == typeName.compare((*it)->getLocalName()))
  //        {
  //          //arrayDepth--;
  //          lastArray = false;
  //          writeVarSerializeMethod(pHeaderFile, (*it), toplevelName,
  //          arrayDepth, lastArray, coladialect);
  //        }
  //      }
  //    }
  //    else
  //    {
  //      string prefix;
  //      if (0 == typeName.compare("String"))
  //      {
  //        prefix = "";
  //      }
  //      else
  //      {
  //        prefix = coladialect + "Serializer::";
  //      }

  //      if (!lastArray)
  //      {
  //        *pHeaderFile << indent << prefix + "Serialize(pDest, rSrc." <<
  //        toplevelName << pType->getLocalName() << " , pOffset);" << endl;
  //      }
  //      else
  //      {
  //        CounterName = arrayDepthToCounter(arrayDepth - 1);
  //        toplevelName.pop_back();
  //        *pHeaderFile << indent << prefix + "Serialize(pDest, rSrc." <<
  //        toplevelName << ", pOffset);" << endl;
  //      }
  //    }
  //    break;
  //  }
  //  default:
  //    break;
  //}
}
//===========================================================================
//===========================================================================
void Writer::writeVarDeserializeMethod(ofstream* pHeaderFile,
                                       GenericCoLaTypeContainer* pType,
                                       std::string& toplevelName,
                                       int32_t arrayDepth, bool lastArray,
                                       std::string coladialect) {
  string CounterName = arrayDepthToCounter(arrayDepth);
  string indent = getIndent(arrayDepth);

  TypeClass tc = pType->GetTypeClass();

  switch (tc) {
    case STRUCT_CONTAINER: {
      vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();
      *pHeaderFile << endl;

      for (vector<GenericCoLaTypeContainer*>::iterator it =
               subTypeList.begin();
           it != subTypeList.end(); ++it) {
        lastArray = false;
        writeVarDeserializeMethod(pHeaderFile, (*it), toplevelName, arrayDepth,
                                  lastArray, coladialect);
      }
      break;
    }
    case XBYTE_CONTAINER: {
      string prefix = coladialect + "Deserializer::";
      *pHeaderFile << indent << "for(int32_t " << CounterName << "=0;"
                   << CounterName << "<"
                   << (dynamic_cast<XByteContainer*>(pType))->GetWidthInBytes()
                   << ";++" << CounterName << ")" << endl;
      *pHeaderFile << indent << "{" << endl;
      *pHeaderFile << indent << "\t" << prefix + "Deserialize(pSrc, rDest."
                   << toplevelName << pType->GetLocalName()
                   << "[" + CounterName + "],";
      *pHeaderFile << "pOffset);" << endl;
      *pHeaderFile << indent << "}" << endl;
      break;
    }
    case ARRAY_CONTAINER: {
      arrayDepth++;
      std::vector<GenericCoLaTypeContainer*> subTypeList =
          pType->GetChildren();

      if ((dynamic_cast<ArrayContainer*>(pType))->IsFlexArray()) {
        *pHeaderFile << indent
                     << coladialect +
                            "Deserializer::Deserialize(pSrc, rDest.uiLength" +
                            pType->GetLocalName() + ", pOffset);"
                     << endl;
        *pHeaderFile << indent << "for(int32_t " << CounterName << "=0;"
                     << CounterName << "<"
                     << "rDest.uiLength" + pType->GetLocalName() << ";++"
                     << CounterName << ")" << endl;
      } else {
        *pHeaderFile << indent << "for(int32_t " << CounterName << "=0;"
                     << CounterName << "<"
                     << (dynamic_cast<ArrayContainer*>(pType))->GetArraySize()
                     << ";++" << CounterName << ")" << endl;
      }

      *pHeaderFile << indent << "{" << endl;
      for (vector<GenericCoLaTypeContainer*>::iterator it =
               subTypeList.begin();
           it != subTypeList.end(); ++it) {
        lastArray = true;
        writeVarDeserializeMethod(
            pHeaderFile, (*it),
            toplevelName + pType->GetLocalName() + "[" + CounterName + "].",
            arrayDepth, lastArray, coladialect);
      }
      arrayDepth--;
      *pHeaderFile << indent << "}" << endl;

      break;
    }
    case STRING_CONTAINER: {
      if (!(dynamic_cast<StringContainer*>(pType))->isFixLength()) {
        *pHeaderFile << indent << "DeserializeFlexString(pSrc, &rDest.";
        *pHeaderFile << "Text[0],"
                     << "&rDest.uiLength";
        *pHeaderFile
            << ","
            << to_string((dynamic_cast<StringContainer*>(pType))->GetLength())
            << " , pOffset);" << endl;
      } else {
        *pHeaderFile
            << indent << "DeserializeFixString(pSrc, &rDest.Text[0],"
            << to_string((dynamic_cast<StringContainer*>(pType))->GetLength())
            << ", pOffset);" << endl;
      }
      break;
    }
    case ENUM_CONTAINER:
    case CONCRETE_TYPE_CONTAINER: {
      string prefix = coladialect + "Deserializer::";

      if (!lastArray) {
        *pHeaderFile << indent << prefix + "Deserialize(pSrc, rDest."
                     << toplevelName << pType->GetLocalName() << " , pOffset);"
                     << endl;
      } else {
        CounterName = arrayDepthToCounter(arrayDepth - 1);
        toplevelName.pop_back();
        *pHeaderFile << indent << prefix + "Deserialize(pSrc, rDest."
                     << toplevelName << ", pOffset);" << endl;
      }

      break;
    }
    case USER_TYPE_CONTAINER: {
      if (!lastArray) {
        *pHeaderFile << indent << "Deserialize(pSrc, rDest." << toplevelName
                     << pType->GetLocalName() << " , pOffset);" << endl;
      } else {
        CounterName = arrayDepthToCounter(arrayDepth - 1);
        toplevelName.pop_back();
        *pHeaderFile << indent << "Deserialize(pSrc, rDest." << toplevelName
                     << ", pOffset);" << endl;
      }
      break;
    }
    default:
      cout << "UPS" << endl;
      break;
  }
}

void Writer::findElements(
    GenericCoLaTypeContainer* pType, std::string prefix,
    std::vector<tuple<std::string, std::string, std::string, std::string>>&
        addresses,
    uint32_t lvl, uint32_t arrayDepth, bool lastArray) {
  if (0 == pType->GetLocalName().compare("xbState")) {
    cout << "stop" << endl;
  }
  switch (pType->GetTypeClass()) {
    case STRUCT_CONTAINER: {
      vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();
      if (0 != prefix.size() && (lastArray == false)) {
        char ch = prefix.back();
        if (ch == '.') {
          prefix = prefix + pType->GetLocalName();
        } else {
          prefix = prefix + "." + pType->GetLocalName();
        }

      } else if (0 != lvl && (lastArray == false)) {
        if (0 != prefix.size()) {
          prefix = prefix + "." + pType->GetLocalName();
        } else {
          prefix = pType->GetLocalName();
        }
      }
      for (auto x : subTypeList) {
        findElements(x, prefix, addresses, lvl + 1, arrayDepth, false);
      }
      break;
    }

    case USER_TYPE_CONTAINER: {
      GenericCoLaTypeContainer* pL;
      pL = pDev_->GetTypeByBaseName(pType->GetBaseTypeName());

      if (pL->GetTypeClass() == STRUCT_CONTAINER) {
        pL->SetLocalName(pType->GetLocalName());
      }

      cout << "User type: " << prefix << endl;
      findElements(pL, prefix, addresses, lvl, arrayDepth, lastArray);
      break;
    }
    case ARRAY_CONTAINER: {
      ArrayContainer* pA = dynamic_cast<ArrayContainer*>(pType);
      vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();

      if (pA->IsFlexArray()) {
        string name = pType->GetLocalName();
        // uiLengthScanData_aEncoderBlock
        if (prefix.size() == 0) {
          name = "uiLength" + pType->GetLocalName();
        } else {
          char ch = prefix.back();
          if (ch == '.') {
            name = prefix + "uiLength" + pType->GetLocalName();
          } else {
            name = prefix + ".uiLength" + pType->GetLocalName();
          }
        }

        cout << "Basic type: " << name << endl;
        tuple<std::string, std::string, std::string, std::string> v = {
            string("uint16_t"), name, to_string(pA->GetArraySize()),
            to_string(pA->GetArraySize())};
        addresses.push_back(v);
      }

      if (0 != prefix.size()) {
        prefix = prefix + "." + pType->GetLocalName() + "[";
      } else {
        prefix = prefix + pType->GetLocalName() + "[";
      }

      for (uint32_t i = 0; i < pA->GetArraySize(); i++) {
        string tmp = prefix + to_string(i) + "]";

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
      if (0 != prefix.size()) {
        if (lastArray == false) {
          prefix = prefix + "." + pType->GetLocalName();
        }
      } else {
        if (lvl != 0) {
          prefix = pType->GetLocalName();
        } else {
        }
      }
      cout << "Basic type: " << prefix << endl;

      tuple<std::string, std::string, std::string, std::string> v = {
          pType->GetBaseTypeName(), prefix, pType->GetMin(), pType->GetMax()};

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

//===========================================================================
//===========================================================================
void Writer::WriteVariableSetterCaseImplementation(std::ofstream* pSourceFile,
                                                   string caseName,
                                                   string element, string min,
                                                   string max) {
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
void Writer::WriteVariableGetterCaseImplementation(std::ofstream* pSourceFile,
                                                   string caseName,
                                                   string element) {
  *pSourceFile << "\t\tcase hash_64_fnv1a_const(\"" + caseName + "\"):" << endl;
  *pSourceFile << "\t\t\tvalue = this->Value_." + element + ";" << endl;
  *pSourceFile << "\t\t\tret = SSBL_SUCCESS;" << endl;
  *pSourceFile << "\t\t\tbreak;" << endl;
}

//===========================================================================
//===========================================================================
void Writer::writeVariableGetterImplementations(std::ofstream* pSourceFile,
                                                CoLaVariable* pVar,
                                                std::string& baseTypeName,
                                                vector<string> addrs) {
  *pSourceFile << "SensorResult ";
  *pSourceFile << pVar->getName() + "_" + pDev_->GetDeviceName() +
                      "Skeleton_Var::";
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
void Writer::writeSimpleVariableGetterImplementations(
    std::ofstream* pSourceFile, CoLaVariable* pVar, std::string& baseTypeName,
    vector<string> addrs) {
  *pSourceFile << "SensorResult ";
  *pSourceFile << pVar->getName() + "_" + pDev_->GetDeviceName() +
                      "Skeleton_Var::";
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
void Writer::writeSimpleVariableSetterImplementations(
    std::ofstream* pSourceFile, CoLaVariable* pVar, std::string& baseTypeName,
    vector<tuple<std::string, std::string, std::string>> addrs) {
  *pSourceFile << "SensorResult ";
  *pSourceFile << pVar->getName() + "_" + pDev_->GetDeviceName() +
                      "Skeleton_Var::";
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
void Writer::writeVariableSetterImplementations(
    std::ofstream* pSourceFile, CoLaVariable* pVar, std::string& baseTypeName,
    vector<tuple<std::string, std::string, std::string>> addrs) {
  *pSourceFile << "SensorResult ";
  *pSourceFile << pVar->getName() + "_" + pDev_->GetDeviceName() +
                      "Skeleton_Var::";
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
void Writer::writeVariableGetterSetterImplementations(
    std::ofstream* pSourceFile, CoLaVariable* pVar) {
  std::vector<tuple<std::string, std::string, std::string, std::string>> elems;
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

  *pSourceFile << pVar->getName() + "_" + pDev_->GetDeviceName() +
                      "Skeleton_Var::SetBasicFromString(std::string value)"
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
void Writer::writeVariableClassImplementation(ofstream* pSourceFile,
                                              CoLaVariable* pVar) {
  addBlankLine(pSourceFile);
  *pSourceFile << pVar->getName() + "_" + pDev_->GetSkeletonName() + "_Var::";
  *pSourceFile << pVar->getName() + "_" + pDev_->GetSkeletonName() + "_Var()"
               << std::endl;

  *pSourceFile << "\t: " + pDev_->GetSkeletonName() + "_Var(\"";
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
void Writer::writeVariableClassImplementations(std::string& folder) {
  ofstream file;
  string FileString =
      folder + "/" + pDev_->GetSkeletonName() + "_Variables.cpp";

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
    openNamespace(&file, "Dev" + pDev_->GetSkeletonName());
    file << "#define SSBL_LONG_MIN  (-2147483647L - 1)" << endl;
    file << "#define SSBL_LLONG_MIN  (-9223372036854775807i64 - 1)" << endl;

    addBlankLine(&file);
    vector<CoLaVariable*> vars = pDev_->GeVariables();
    for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
         ++it) {
      writeVariableClassImplementation(&file, (*it));
    }

    closeNamespace(&file, "Dev" + pDev_->GetSkeletonName());
    closeNamespace(&file, "ssbl");
    addBlankLine(&file);
    file.close();
  }
}

//===========================================================================
//===========================================================================
void Writer::writeMethodClassImplementation(std::ofstream* pHeaderFile,
                                            CoLaMethod* pMeth) {
  addBlankLine(pHeaderFile);

  string classSpec;
  if (NULL != pMeth->GetArgType() && NULL != pMeth->GetReturnType()) {
    classSpec = "AR(\"" + pMeth->getName() + "\" , \"" + pMeth->getComName() +
                "\" ," + std::to_string(pMeth->getIndex()) + " , " +
                pMeth->getFinalAccessLevel() + ")";
  } else if (NULL == pMeth->GetArgType() && NULL != pMeth->GetReturnType()) {
    classSpec = "NAR(\"" + pMeth->getName() + "\" , \"" + pMeth->getComName() +
                "\" ," + std::to_string(pMeth->getIndex()) + " , " +
                pMeth->getFinalAccessLevel() + ")";
  } else if (NULL != pMeth->GetArgType() && NULL == pMeth->GetReturnType()) {
    classSpec = "ANR(\"" + pMeth->getName() + "\" , \"" + pMeth->getComName() +
                "\" ," + std::to_string(pMeth->getIndex()) + " , " +
                pMeth->getFinalAccessLevel() + ")";
  } else {
    classSpec = "NANR(\"" + pMeth->getName() + "\" , \"" + pMeth->getComName() +
                "\" ," + std::to_string(pMeth->getIndex()) + " , " +
                pMeth->getFinalAccessLevel() + ")";
  }

  *pHeaderFile << pMeth->getName() + "_" + pDev_->GetSkeletonName() + "_Func::";
  *pHeaderFile << pMeth->getName() + "_" + pDev_->GetSkeletonName() + "_Func()"
               << std::endl;

  *pHeaderFile << "\t: " + pDev_->GetSkeletonName() + "_Func_" + classSpec
               << std::endl;

  *pHeaderFile << "{" << std::endl;
  *pHeaderFile << "}" << std::endl;
  addBlankLine(pHeaderFile);
}

//===========================================================================
//===========================================================================
void Writer::writeMethodClassImplementations(std::string& folder) {
  ofstream file;
  string FileString =
      folder + "/" + pDev_->GetSkeletonName() + "_Functions.cpp";

  file.open(FileString.c_str(), ios::out | ios::trunc);

  if (file.is_open()) {
    writeNoticeHeader(&file);
    addBlankLine(&file);
    addLocalIncludeFile(&file, "_Functions.h");
    addLocalIncludeFile(&file, "_Variables.h");

    addBlankLine(&file);

    openNamespace(&file, "ssbl");
    openNamespace(&file, "Dev" + pDev_->GetSkeletonName());

    vector<CoLaMethod*> meth = pDev_->GetMethods();
    for (vector<CoLaMethod*>::iterator it = meth.begin(); it != meth.end();
         ++it) {
      writeMethodClassImplementation(&file, (*it));
    }
    // end namespace and close file
    closeNamespace(&file, "Dev" + pDev_->GetSkeletonName());
    closeNamespace(&file, "ssbl");

    file.close();
  }
}

//===========================================================================
//===========================================================================
void Writer::writeSerializer(std::ofstream* pHeaderFile,
                             std::string ColaDialect) {
  std::vector<int32_t> FlexStringLengths;
  std::vector<int32_t> FixStringLengths;
  addBlankLine(pHeaderFile);

  openNamespace(pHeaderFile, "ssbl");
  openNamespace(pHeaderFile, "Dev" + pDev_->GetSkeletonName());

  openClass(pHeaderFile, pDev_->GetSkeletonName() + "_" + ColaDialect +
                             "Serializer : public " + ColaDialect +
                             "Serializer");
  *pHeaderFile << "\t" + pDev_->GetSkeletonName() + "_" + ColaDialect +
                      "Serializer(){};"
               << endl;
  *pHeaderFile << "\t~" + pDev_->GetSkeletonName() + "_" + ColaDialect +
                      "Serializer(){};"
               << endl;

  addBlankLine(pHeaderFile);
  *pHeaderFile << "\tusing::ssbl::" + ColaDialect + "Serializer::Serialize;"
               << endl;
  addBlankLine(pHeaderFile);

  vector<GenericCoLaTypeContainer*> types = pDev_->GetTypes();
  for (vector<GenericCoLaTypeContainer*>::iterator it = types.begin();
       it != types.end(); ++it) {
    if ((*it)->GetTypeClass() == ARRAY_CONTAINER) {
      *pHeaderFile << "\tvoid Serialize(uint8_t* pDest, " +
                          (*it)->GetBaseTypeName() +
                          "_array_t & rSrc, uint32_t* pOffset);"
                   << endl;
    } else {
      *pHeaderFile << "\tvoid Serialize(uint8_t* pDest, " +
                          (*it)->GetBaseTypeName() +
                          " & rSrc, uint32_t* pOffset);"
                   << endl;
    }
  }
  vector<CoLaVariable*> vars = pDev_->GeVariables();
  for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
       ++it) {
    //*pHeaderFile << "\tvoid Serialize(uint8_t* pDest, " + (*it)->getName() +
    //"_vt & rSrc, uint32_t* pOffset);" << endl;
  }
  vector<CoLaMethod*> meth = pDev_->GetMethods();
  for (vector<CoLaMethod*>::iterator it = meth.begin(); it != meth.end();
       ++it) {
    if ((*it)->GetArgType() != NULL) {
      //   *pHeaderFile << "\tvoid Serialize(uint8_t* pDest, Args_" +
      //   ((*it)->GetArgType())->GetLocalName() + "_at & rSrc, uint32_t*
      //   pOffset);" << endl;
    }
  }
  closeClass(pHeaderFile,
             pDev_->GetSkeletonName() + "_" + ColaDialect + "Serializer");
  addBlankLine(pHeaderFile);
  closeNamespace(pHeaderFile, "Dev" + pDev_->GetSkeletonName());
  closeNamespace(pHeaderFile, "ssbl");
  addBlankLine(pHeaderFile);
}
//===========================================================================
//===========================================================================
void Writer::writeSerializerImpl(std::ofstream* pSourceFile,
                                 std::string ColaDialect) {
  std::vector<int32_t> FlexStringLengths;
  std::vector<int32_t> FixStringLengths;

  addBlankLine(pSourceFile);

  openNamespace(pSourceFile, "ssbl");
  openNamespace(pSourceFile, "Dev" + pDev_->GetSkeletonName());

  string front =
      "\tvoid " + pDev_->GetSkeletonName() + "_" + ColaDialect + "Serializer";

  // GetUniqueStringLengths(FixStringLengths, FlexStringLengths);

  string method;
  string x = "";

  vector<GenericCoLaTypeContainer*> types = pDev_->GetTypes();
  for (vector<GenericCoLaTypeContainer*>::iterator it = types.begin();
       it != types.end(); ++it) {
    if ((*it)->GetTypeClass() == ARRAY_CONTAINER) {
      method = "Serialize(uint8_t* pDest, " + (*it)->GetBaseTypeName() +
               "_array_t & rSrc, uint32_t* pOffset)";
    } else {
      method = "Serialize(uint8_t* pDest, " + (*it)->GetBaseTypeName() +
               "& rSrc, uint32_t* pOffset)";
    }
    openClassMethod(pSourceFile, front, method);

    writeVarSerializeMethod(pSourceFile, (*it), x, 0, false, ColaDialect);
    closeClassMethod(pSourceFile);
    addBlankLine(pSourceFile);
  }

  /*
  vector<CoLaVariable*> vars = pDev_->GeVariables();
  for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
  ++it)
  {
    method = "Serialize(uint8_t* pDest, " + (*it)->getName() + "_vt & rSrc,
  uint32_t* pOffset)"; openClassMethod(pSourceFile, front, method);

    writeVarSerializeMethod(pSourceFile, (*it)->getType(), x, 0, false,
  ColaDialect); closeClassMethod(pSourceFile); addBlankLine(pSourceFile);
  }

  vector<CoLaMethod*> meth = pDev_->GetMethods();
  for (vector<CoLaMethod*>::iterator it = meth.begin(); it != meth.end(); ++it)
  {
    if ((*it)->GetArgType() != NULL)
    {
      method = "Serialize(uint8_t* pDest, Args_" +
  ((*it)->GetArgType())->GetLocalName() + "_at & rSrc, uint32_t* pOffset)";
      openClassMethod(pSourceFile, front, method);

      writeVarSerializeMethod(pSourceFile, (*it)->GetArgType(), x, 0, false,
  ColaDialect); closeClassMethod(pSourceFile); addBlankLine(pSourceFile);
    }
  }
  */
  addBlankLine(pSourceFile);
  closeNamespace(pSourceFile, "Dev" + pDev_->GetSkeletonName());
  closeNamespace(pSourceFile, "ssbl");
  addBlankLine(pSourceFile);
}

//===========================================================================
//===========================================================================
void Writer::writeDeserializer(std::ofstream* pHeaderFile,
                               std::string ColaDialect) {
  std::vector<int32_t> FlexStringLengths;
  std::vector<int32_t> FixStringLengths;
  addBlankLine(pHeaderFile);

  openNamespace(pHeaderFile, "ssbl");
  openNamespace(pHeaderFile, "Dev" + pDev_->GetSkeletonName());

  openClass(pHeaderFile, pDev_->GetSkeletonName() + "_" + ColaDialect +
                             "Deserializer : public " + ColaDialect +
                             "Deserializer");
  *pHeaderFile << "\t" + pDev_->GetSkeletonName() + "_" + ColaDialect +
                      "Deserializer(){};"
               << endl;
  *pHeaderFile << "\t~" + pDev_->GetSkeletonName() + "_" + ColaDialect +
                      "Deserializer(){};"
               << endl;

  addBlankLine(pHeaderFile);

  // GetUniqueStringLengths(FixStringLengths, FlexStringLengths);

  *pHeaderFile << "\tusing::ssbl::" + ColaDialect + "Deserializer::Deserialize;"
               << endl;
  addBlankLine(pHeaderFile);

  vector<GenericCoLaTypeContainer*> types = pDev_->GetTypes();
  for (vector<GenericCoLaTypeContainer*>::iterator it = types.begin();
       it != types.end(); ++it) {
    if ((*it)->GetTypeClass() == ARRAY_CONTAINER) {
      *pHeaderFile << "\tvoid Deserialize(uint8_t* pSrc, " +
                          (*it)->GetBaseTypeName() +
                          "_array_t & rDest, uint32_t* pOffset);"
                   << endl;
    } else {
      *pHeaderFile << "\tvoid Deserialize(uint8_t* pSrc, " +
                          (*it)->GetBaseTypeName() +
                          "& rDest, uint32_t* pOffset);"
                   << endl;
    }
  }

  vector<CoLaVariable*> vars = pDev_->GeVariables();
  for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
       ++it) {
    //*pHeaderFile << "\tvoid Deserialize(uint8_t* pSrc, " + (*it)->getName() +
    //"_vt & rDest, uint32_t* pOffset);" << endl;
  }

  vector<CoLaMethod*> meth = pDev_->GetMethods();
  for (vector<CoLaMethod*>::iterator it = meth.begin(); it != meth.end();
       ++it) {
    if ((*it)->GetReturnType() != NULL) {
      //  *pHeaderFile << "\tvoid Deserialize(uint8_t* pSrc, RetVal_" +
      //  ((*it)->GetReturnType())->GetLocalName() + "_rt & rDest, uint32_t*
      //  pOffset);" << endl;
    }
  }

  closeClass(pHeaderFile,
             pDev_->GetSkeletonName() + "_" + ColaDialect + "Deserializer");
  addBlankLine(pHeaderFile);
  closeNamespace(pHeaderFile, "Dev" + pDev_->GetSkeletonName());
  closeNamespace(pHeaderFile, "ssbl");
  addBlankLine(pHeaderFile);
}

//===========================================================================
//===========================================================================
void Writer::writeDeserializerImpl(std::ofstream* pSourceFile,
                                   std::string ColaDialect) {
  std::vector<int32_t> FlexStringLengths;
  std::vector<int32_t> FixStringLengths;

  addBlankLine(pSourceFile);

  openNamespace(pSourceFile, "ssbl");
  openNamespace(pSourceFile, "Dev" + pDev_->GetSkeletonName());

  string front =
      "\tvoid " + pDev_->GetSkeletonName() + "_" + ColaDialect + "Deserializer";

  string method;
  string x = "";

  vector<GenericCoLaTypeContainer*> types = pDev_->GetTypes();
  for (vector<GenericCoLaTypeContainer*>::iterator it = types.begin();
       it != types.end(); ++it) {
    if ((*it)->GetTypeClass() == ARRAY_CONTAINER) {
      method = "Deserialize(uint8_t* pSrc, " + (*it)->GetBaseTypeName() +
               "_array_t & rDest, uint32_t* pOffset)";
    } else {
      method = "Serialize(uint8_t* pDest, " + (*it)->GetBaseTypeName() +
               "& rSrc, uint32_t* pOffset)";
      method = "Deserialize(uint8_t* pSrc, " + (*it)->GetBaseTypeName() +
               "& rDest, uint32_t* pOffset)";
    }
    openClassMethod(pSourceFile, front, method);

    writeVarDeserializeMethod(pSourceFile, (*it), x, 0, false, ColaDialect);
    closeClassMethod(pSourceFile);
    addBlankLine(pSourceFile);
  }
  /*
  vector<CoLaVariable*> vars = pDev_->GeVariables();
  for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
  ++it)
  {
    method = "Deserialize(uint8_t* pSrc, " + (*it)->getName() + "_vt & rDest,
  uint32_t* pOffset)"; openClassMethod(pSourceFile, front, method);

    writeVarDeserializeMethod(pSourceFile, (*it)->getType(), x, 0, false,
  ColaDialect); closeClassMethod(pSourceFile); addBlankLine(pSourceFile);
  }

  vector<CoLaMethod*> meth = pDev_->GetMethods();
  for (vector<CoLaMethod*>::iterator it = meth.begin(); it != meth.end(); ++it)
  {
    if ((*it)->GetReturnType() != NULL)
    {
      method = "Deserialize(uint8_t* pSrc, RetVal_" +
  ((*it)->GetReturnType())->GetLocalName() + "_rt & rDest, uint32_t* pOffset)";
      openClassMethod(pSourceFile, front, method);

      writeVarDeserializeMethod(pSourceFile, (*it)->GetReturnType(), x, 0,
  false, ColaDialect); closeClassMethod(pSourceFile); addBlankLine(pSourceFile);
    }
  }
  */

  addBlankLine(pSourceFile);
  closeNamespace(pSourceFile, "Dev" + pDev_->GetSkeletonName());
  closeNamespace(pSourceFile, "ssbl");
  addBlankLine(pSourceFile);
}

//===========================================================================
//===========================================================================
void Writer::writeColaExtensionFiles(std::string& includefolder,
                                     std::string& sourcefolder) {
  ofstream sourcefile, headerfile;
  string sourceFileString =
      sourcefolder + "/" + pDev_->GetSkeletonName() + "_CoLa_Extension.cpp";
  string headerFileString =
      includefolder + "/" + pDev_->GetSkeletonName() + "_CoLa_Extension.h";
  sourcefile.open(sourceFileString.c_str(), ios::out | ios::trunc);
  headerfile.open(headerFileString.c_str(), ios::out | ios::trunc);

  std::vector<SensorInterface*> iFaces = pDev_->GetInterfaces();
  vector<std::string> done;

  if (sourcefile.is_open()) {
    createSourceHeader(&sourcefile);
    addLocalIncludeFile(&sourcefile, string("_") + "CoLa_Extension.h");
  }

  std::vector<SensorInterfaceProtocol*> iFaceProtos = pDev_->GetProtocols();

  if (headerfile.is_open()) {
    createHeaderHeader(&headerfile);

    addSystemIncludeFile(&headerfile, "stdint.h");

    addLocalIncludeFile(&headerfile, string("_") + "Types.h");
    for (vector<SensorInterfaceProtocol*>::iterator it = iFaceProtos.begin();
         it != iFaceProtos.end(); ++it) {
      if (std::find(done.begin(), done.end(), (*it)->GetProtocolType()) ==
          done.end()) {
        string out;
        if (0 == ((*it)->GetProtocolType()).compare("COLA_A")) {
          out = "CoLaA";
        }
        if (0 == ((*it)->GetProtocolType()).compare("COLA_B")) {
          out = "CoLaB";
        }
        if (0 == ((*it)->GetProtocolType()).compare("COLA_2")) {
          out = "CoLa2";
        }
        addProjectIncludeFile(&headerfile, "Base/Protocol/include/" +
                                               out.substr(0, out.size() - 1) +
                                               "/" + out + "Deserializer.h");
        addProjectIncludeFile(&headerfile, "Base/Protocol/include/" +
                                               out.substr(0, out.size() - 1) +
                                               "/" + out + "Serializer.h");
        addProjectIncludeFile(&headerfile, "Base/Protocol/include/" +
                                               out.substr(0, out.size() - 1) +
                                               "/" + out + "Protocol.h");
        done.push_back((*it)->GetProtocolType());
      }
    }
  }

  done.clear();

  for (vector<SensorInterfaceProtocol*>::iterator it = iFaceProtos.begin();
       it != iFaceProtos.end(); ++it) {
    if (std::find(done.begin(), done.end(), (*it)->GetProtocolType()) ==
        done.end()) {
      string out;
      if (0 == ((*it)->GetProtocolType()).compare("COLA_A")) {
        out = "CoLaA";
      }
      if (0 == ((*it)->GetProtocolType()).compare("COLA_B")) {
        out = "CoLaB";
      }
      if (0 == ((*it)->GetProtocolType()).compare("COLA_2")) {
        out = "CoLa2";
      }
      writeColaExtension(includefolder, sourcefolder, out);
      done.push_back((*it)->GetProtocolType());
    }
  }
}

//===========================================================================
//===========================================================================
void Writer::writeColaExtension(std::string& includefolder,
                                std::string& sourcefolder,
                                std::string ColaDialect) {
  ofstream sourcefile, headerfile;
  string sourceFileString =
      sourcefolder + "/" + pDev_->GetSkeletonName() + "_CoLa_Extension.cpp";
  string headerFileString =
      includefolder + "/" + pDev_->GetSkeletonName() + "_CoLa_Extension.h";
  sourcefile.open(sourceFileString.c_str(), ios::out | ios::app);
  headerfile.open(headerFileString.c_str(), ios::out | ios::app);

  if (sourcefile.is_open()) {
    addBlankLine(&sourcefile);
    writeSerializerImpl(&sourcefile, ColaDialect);
    writeDeserializerImpl(&sourcefile, ColaDialect);
    sourcefile.close();
  }

  if (headerfile.is_open()) {
    addBlankLine(&headerfile);

    writeSerializer(&headerfile, ColaDialect);
    writeDeserializer(&headerfile, ColaDialect);

    addBlankLine(&headerfile);

    openNamespace(&headerfile, "ssbl");
    openNamespace(&headerfile, "Dev" + pDev_->GetSkeletonName());
    openClass(&headerfile, pDev_->GetSkeletonName() + "_" + ColaDialect +
                               "Protocol : public " + ColaDialect + "Protocol");
    headerfile << "\t" + pDev_->GetSkeletonName() + "_" + ColaDialect +
                      "Protocol(AddressingMode AddrMode,"
                      "size_t txBufSize, size_t rxBufSize)"
               << endl;
    headerfile << "\t\t:" + ColaDialect +
                      "Protocol(AddrMode, txBufSize, rxBufSize)"
               << endl;
    headerfile << "\t{" << endl;
    headerfile << "\t\tpSerializer_ = &Serializer_;" << endl;
    headerfile << "\t\tpDeserializer_ = &Deserializer_;" << endl;
    headerfile << "\t};" << endl;
    headerfile << "\t~" + pDev_->GetSkeletonName() + "_" + ColaDialect +
                      "Protocol(){};"
               << endl;
    headerfile << "\tprivate:" << endl;
    headerfile << "\t\t" + pDev_->GetSkeletonName() + "_" + ColaDialect +
                      "Serializer Serializer_;"
               << endl;
    headerfile << "\t\t" + pDev_->GetSkeletonName() + "_" + ColaDialect +
                      "Deserializer Deserializer_;"
               << endl;
    headerfile << "\t};" << endl;

    addBlankLine(&headerfile);
    closeNamespace(&headerfile, "Dev" + pDev_->GetSkeletonName());
    closeNamespace(&headerfile, "ssbl");
    addBlankLine(&headerfile);

    headerfile.close();
  }
}

//===========================================================================
//===========================================================================
void Writer::writeSickSensorVarExtension(
    std::string& SickSensorVarIncludeFile) {
  fstream headerfile;
  string line;
  std::string buffer;
  headerfile.open(SickSensorVarIncludeFile.c_str(), ios::in);
  if (headerfile.is_open()) {
    std::stringstream strStream;
    strStream << headerfile.rdbuf();
    buffer = strStream.str();
    headerfile.close();
  }

  size_t pos =
      buffer.find("//Generated code ends here - do not remove this comment");

  if (pos == string::npos) {
    cout << "Error no //Generated code ends here - do not remove this comment "
            "found in file "
         << SickSensorVarIncludeFile << endl;
  } else {
    std::string front = buffer.substr(0, pos);
    vector<CoLaVariable*> vars = pDev_->GeVariables();
    for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
         ++it) {
      string out = "\tclass ";
      out = out + (*it)->getName() + "_" + pDev_->GetDeviceName() +
            "Skeleton_Var";
      out = out + ";\n";
      front.append(out);
      out = "\tvirtual " + (*it)->getName() + "_" + pDev_->GetDeviceName() +
            "Skeleton_Var * Downcast(){return NULL;};";
      front.append(out);
    }
    std::string end = buffer.substr(pos);
    headerfile.open(SickSensorVarIncludeFile.c_str(), ios::out | ios::trunc);
    if (headerfile.is_open()) {
      headerfile << front;
      headerfile << end;
      headerfile.close();
    }
  }

  /*
  while (std::getline(headerfile, line))
  {
    if (string::npos != line.find("//Generated code starts here - do not remove
  this comment"))
    {
      std::string buffer;
      headerfile.seekp(headerfile.tellg());

      headerfile.seekg(headerfile.tellg(), std::ios::end);
      
































































      buffer.reserve(headerfile.tellg());

      buffer.assign((std::istreambuf_iterator<char>(headerfile)),
        std::istreambuf_iterator<char>());

      headerfile << "class" << endl;
      headerfile << buffer;
      break;
    }
  }
  */
  //   headerfile.close();
  // }
}