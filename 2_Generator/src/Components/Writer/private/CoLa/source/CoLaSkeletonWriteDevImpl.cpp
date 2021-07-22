#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/CoLa/CoLaSkeletonWriter.h"

//CoLaSkeletonWriter
using namespace std;

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeDeviceClassImplementationFile(const string& folder) {
  ofstream file;
  string FileString =
      folder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      ".cpp";

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
    openNamespace(&file,
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetSkeletonName());
    addBlankLine(&file);

    file << "//TODO: Buffer size is set fixed" << endl;
    file << "//TODO: default interface" << endl;
    file << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
               "::" +
               static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                    ->GetDeviceName();
    file << "(const std::string& Ip, const std::string& interfaceName,const "
            "std::string& localName)"
         << endl;
    file << "\t: CoLaSensorSkeleton(localName,";
    file << "16 * 1024,16 * 1024";

    std::vector<CoLaUserLevel*> levels =
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetUserLevels();
    file << ")" << endl;
    file << "{" << endl;

    file << "\tSensorName_=\"" +
                static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                    ->GetDeviceName() +
                "\";"
         << endl;

    file << "\tpasswords_[LEVEL_INVALID] = 0;" << endl;

    for (vector<CoLaUserLevel*>::iterator it = levels.begin();
         it != levels.end(); ++it) {
      file << "\tpasswords_[" << (*it)->getUserLvlName();
      file << "] = " << (*it)->getUserLvlPassword() << ";" << endl;
    }
    file << "\tBehavorialVersion_ = \"" <<    pDevFamily_->GetBehavior() << "\";"    << endl;
    file << "\tUserSelectedIpOrSerial_ = Ip;" << endl;
    file << "\tUserSelectedInterfaceName_ = interfaceName;" << endl;

    std::vector<SensorInterface*> iFaces =
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetInterfaces();

    for (vector<SensorInterface*>::iterator it = iFaces.begin();
         it != iFaces.end(); ++it) {
      std::vector<SensorInterfaceProtocol*> iFaceProtos = (*it)->GetProtocols();
      for (vector<SensorInterfaceProtocol*>::iterator iit = iFaceProtos.begin();
           iit != iFaceProtos.end(); ++iit) {
        file << "\tAvailableSensorInterfaces_.push_back(ssbl::make_unique<"
                "SensorInterfaceDescription*>( new "
                "SensorInterfaceDescription(";
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
    vector<CoLaVariable*> vars =
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetVariables();
    for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
         ++it) {
      file << "\tVariableRepo.RegisterComObj(\"" +
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                      ->GetDeviceName() +
                  (*it)->getName() + "\",";
      file << (*it)->getName() + "_" +
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                      ->GetDeviceName() +
                  "_Var::Create); "
           << endl;
    }
    vector<CoLaMethod*> mths =
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetMethods();
    for (vector<CoLaMethod*>::iterator it = mths.begin(); it != mths.end();
         ++it) {
      file << "\tFunctionRepo.RegisterComObj(\"" +
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                      ->GetDeviceName() +
                  (*it)->getName() + "\",";
      file << (*it)->getName() + "_" +
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                      ->GetDeviceName() +
                  "_Func::Create); "
           << endl;
    }

    file << "}" << endl;
    addBlankLine(&file);
    file << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
               "::~" +
               static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                    ->GetDeviceName() +
               "()"
         << endl;
    file << "{" << endl;
    file << "\tfor (auto& x : AvailableSensorInterfaces_) {" << endl;
    file << "\t\tSensorInterfaceDescription* p = *(x.release());" << endl;
    file << "\t\tdelete p;" << endl;
    file << "\t}" << endl;
    file << "\tAvailableSensorInterfaces_.clear();" << endl;
    file << "}" << endl;

    addBlankLine(&file);
    file << "Protocol * " +
                static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                    ->GetDeviceName() +
                "::DeviceSpecificProtocolInit(ProtocolType Protocol, "
                "AddressingMode AddrMode)"
         << endl;
    file << "{" << endl;
    file << "\tssbl::Protocol * pRet = NULL;" << endl;
    file << "\tswitch (Protocol)" << endl;
    file << "\t{" << endl;

    vector<std::string> done;

    std::vector<SensorInterfaceProtocol*> iFaceProtos =
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetProtocols();

    for (vector<SensorInterfaceProtocol*>::iterator it = iFaceProtos.begin();
         it != iFaceProtos.end(); ++it) {
      if (std::find(done.begin(), done.end(), (*it)->GetProtocolType()) ==
          done.end()) {
        file << "\tcase " << (*it)->GetProtocolType() + ":" << endl;
        file << "\t\tpRet = new " +
                    static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                        ->GetDeviceName() +
                    "_";

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
    file << "SensorResult " +
                static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                    ->GetDeviceName() +
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

    file << "SensorResult " +
                static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                    ->GetDeviceName() +
                "::StoreParameter()"
         << endl;
    file << "{" << endl;
    file << "\tSensorResult ret = SSBL_ERR_FEATURE_NOT_SUPPORTED;" << endl;

    if (static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->StoreParamGiven()) {
      file << "\tauto m = CreateFunction(\"" +
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                      ->GetStoreParamFuncName() +
                  "\");"
           << endl;
      file << "\tret = CoLaSensorSkeleton::CallFunction(*m);" << endl;
      file << "\tSSBL_Sleep("
           << to_string(static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                            ->GetStoreParamFuncTime())
           << ");" << endl;
      file << "\tdelete m;" << endl;
    }
    file << "\treturn ret;" << endl;
    file << "}" << endl;
    addBlankLine(&file);

    file << "SensorResult " +
                static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                    ->GetDeviceName() +
                "::RebootSensor()"
         << endl;
    file << "{" << endl;
    file << "\tSensorResult ret = SSBL_ERR_FEATURE_NOT_SUPPORTED;" << endl;

    if (static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->RebootGiven()) {
      file << "\tauto m = CreateFunction(\"" +
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                      ->GetRebootFuncName() +
                  "\");"
           << endl;
      file << "\tret = CoLaSensorSkeleton::CallFunction(*m);" << endl;

      file << "\tSSBL_Sleep("
           << to_string(static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                            ->GetRebootFuncTime())
           << ");"
           << endl;

      file << "\tdelete m;" << endl;
    }
    file << "\treturn ret;" << endl;
    file << "}" << endl;

    closeNamespace(&file,
                   static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                               ->GetSkeletonName());
    closeNamespace(&file, "ssbl");
    file.close();
  }
}