#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/CoLa/CoLaSkeletonWriter.h"

using namespace std;
using namespace std::filesystem;


//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeDeviceClassDefinition(ofstream* pHeaderFile) {
  *pHeaderFile << endl;

  *pHeaderFile
      << "class "
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
               << " : public CoLaSensorSkeleton" << endl;
  *pHeaderFile << "{" << endl << "public:" << endl;

  std::vector<SensorInterface*> iFaces =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetInterfaces();
  string defaultName = "";
  for (vector<SensorInterface*>::iterator it = iFaces.begin();
       it != iFaces.end(); ++it) {
    if ((*it)->IsDefaultInterface()) {
      defaultName = (*it)->GetName();
    }
  }

  *pHeaderFile
      << "\t"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
               << "(const std::string& Ip, const std::string& interfaceName = "
                  "std::string(\"";
  *pHeaderFile << defaultName +
                      "\"),const std::string& localName = std::string(\""
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
      << "\"));" << endl;
  *pHeaderFile
      << "\t~"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
      << "();" << endl;

  *pHeaderFile << "\ttemplate <class T>" << endl;
  *pHeaderFile
      << "\tSensorResult ReadVariable("
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName();
  *pHeaderFile << "_Var<T> & rVar)" << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile << "\t\treturn CoLaSensorSkeleton::ReadVariable(rVar);" << endl;
  *pHeaderFile << "\t}" << endl;

  addBlankLine(pHeaderFile);
  *pHeaderFile << "\tProtocol * DeviceSpecificProtocolInit(ProtocolType "
                  "Protocol, AddressingMode AddrMode);"
               << endl;

  addBlankLine(pHeaderFile);
  *pHeaderFile << "\tSensorResult DeviceSpecificProtocolSwitch(ProtocolType "
                  "Protocol);"
               << endl;

  addBlankLine(pHeaderFile);

  *pHeaderFile << "\ttemplate <class T>" << endl;
  *pHeaderFile
      << "\tSensorResult WriteVariable("
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName();
  *pHeaderFile << "_Var<T> & rVar)" << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile << "\t\treturn CoLaSensorSkeleton::WriteVariable(rVar);" << endl;
  *pHeaderFile << "\t}" << endl;
  addBlankLine(pHeaderFile);

  std::vector<string> classTypes = {"NANR", "ANR<Args>", "NAR<Return>",
                                    "AR<Args, Return>"};
  std::vector<string> header = {"", "template<class Args>",
                                "template<class Return>",
                                "template <class Args, class Return>"};
  for (int32_t i = 0; i < 4; i++) {
    *pHeaderFile << "\t" << header[i] << endl;
    *pHeaderFile << "\t#if defined(__clang__)" << endl;
    *pHeaderFile << "\t\t#pragma clang diagnostic push" << endl;
    *pHeaderFile << "\t\t#pragma clang diagnostic ignored \"-Woverloaded-virtual\""  << endl;
    *pHeaderFile << "\t#endif" << endl;
    *pHeaderFile << "\tSensorResult CallFunction(" +
                        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                            ->GetDeviceName() +
                        "_Func_";
    *pHeaderFile << classTypes[i] + " & rFunc)" << endl;
    *pHeaderFile << "\t{" << endl;
    *pHeaderFile << "\t\treturn CoLaSensorSkeleton::CallFunction(rFunc);"
                 << endl;
    *pHeaderFile << "\t}" << endl;
    *pHeaderFile << "\t#if defined(__clang__)" << endl;
    *pHeaderFile << "\t\t#pragma clang diagnostic pop" << endl;
    *pHeaderFile << "\t#endif" << endl;
    addBlankLine(pHeaderFile);
  }

  *pHeaderFile << "\ttemplate <class T>" << endl;
  *pHeaderFile
      << "\tSensorResult RegisterEvent("
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName();
  *pHeaderFile << "_Var<T>& rVar, std::function<void(uint64_t*)> OnEventCb, "
                  "uint64_t cbParam)"
               << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile
      << "\t\treturn CoLaSensorSkeleton::RegisterEvent(rVar,OnEventCb,cbParam);"
      << endl;
  *pHeaderFile << "\t}" << endl;

  addBlankLine(pHeaderFile);

  *pHeaderFile << "\tSensorResult RegisterEvent(";
  *pHeaderFile << "const std::string & varName, std::function<void(uint64_t*)> "
                  "OnEventCb, uint64_t cbParam)"
               << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile
      << "\t\treturn "
         "CoLaSensorSkeleton::RegisterEvent(varName,OnEventCb,cbParam);"
      << endl;
  *pHeaderFile << "\t}" << endl;

  addBlankLine(pHeaderFile);

  *pHeaderFile << "\ttemplate <class T>" << endl;
  *pHeaderFile
      << "\tSensorResult RegisterEvent("
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName();
  *pHeaderFile
      << "_Var<T>& rVar, VariableEventQueue** ppQueue, uint32_t nQueueElem)"
      << endl;

  *pHeaderFile << "\t{" << endl;
  *pHeaderFile << "\t\treturn "
                  "CoLaSensorSkeleton::RegisterEvent(rVar,ppQueue,nQueueElem);"
               << endl;
  *pHeaderFile << "\t}" << endl;
  addBlankLine(pHeaderFile);

  *pHeaderFile << "\tSensorResult RegisterEvent(";
  *pHeaderFile << "const std::string & varName,VariableEventQueue** ppQueue, "
                  "uint32_t nQueueElem)"
               << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile
      << "\t\treturn "
         "CoLaSensorSkeleton::RegisterEvent(varName,ppQueue,nQueueElem);"
      << endl;
  *pHeaderFile << "\t}" << endl;
  addBlankLine(pHeaderFile);

  *pHeaderFile << "\ttemplate <class T>" << endl;
  *pHeaderFile
      << "\tSensorResult DeregisterEvent("
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName();
  *pHeaderFile << "_Var<T>& rVar, bool isDisconneted = false)" << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile << "\t\treturn CoLaSensorSkeleton::DeregisterEvent(rVar, isDisconneted);"
               << endl;
  *pHeaderFile << "\t}" << endl;
  addBlankLine(pHeaderFile);
  *pHeaderFile << "\tSensorResult DeregisterEvent(";
  *pHeaderFile << "const std::string & varName, bool isDisconneted = false)" << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile << "\t\treturn CoLaSensorSkeleton::DeregisterEvent(varName, isDisconneted);"
               << endl;
  *pHeaderFile << "\t}" << endl;
  addBlankLine(pHeaderFile);
  *pHeaderFile << "\tSensorResult StoreParameter();" << endl;
  *pHeaderFile << "\tSensorResult RebootSensor();" << endl;
  *pHeaderFile << "};" << endl;

  *pHeaderFile << endl;
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeDeviceClassDefinitionFile(const string& folder) {
  ofstream deviceFile;
  string headerFileString =
      folder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      ".h";

  deviceFile.open(headerFileString.c_str(), ios::out | ios::trunc);

  if (deviceFile.is_open()) {
    writeNoticeHeader(&deviceFile);
    addBlankLine(&deviceFile);
    deviceFile << "#pragma once" << endl;

    addSystemIncludeFile(&deviceFile, "stdint.h");
    addLocalIncludeFile(&deviceFile, "_Variables.h");
    addLocalIncludeFile(&deviceFile, "_Functions.h");
    addLocalIncludeFile(&deviceFile, "_Factory.h");
    addProjectIncludeFile(&deviceFile,
                          "Base/Core/Sensor/include/CoLa/CoLaSensorSkeleton.h");
    addProjectIncludeFile(&deviceFile,
                          "Base/Core/OS/include/VariableEventQueue.h");

    addBlankLine(&deviceFile);

    openNamespace(&deviceFile, "ssbl");
    openNamespace(&deviceFile,
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetSkeletonName());

    writeDeviceClassDefinition(&deviceFile);

    // end namespace and close file
    closeNamespace(&deviceFile,
                   static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                               ->GetSkeletonName());
    closeNamespace(&deviceFile, "ssbl");
    addBlankLine(&deviceFile);
    deviceFile.close();
  }
}
