#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/CoLa/CoLaSkeletonWriter.h"

using namespace std;

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableDefinitionsFile(const string& outputFolder) {

  ofstream varFile;
  string headerFileString =
    outputFolder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
    "_Variables.h";


  varFile.open(headerFileString.c_str(), ios::out | ios::trunc);

  if (varFile.is_open()) {
    writeNoticeHeader(&varFile);
    addBlankLine(&varFile);
    varFile << "#pragma once" << endl;

    addSystemIncludeFile(&varFile, "stdint.h");
    addSystemIncludeFile(&varFile, "cstring");
    addLocalIncludeFile(&varFile, "_Types.h");
    addProjectIncludeFile(
        &varFile, "Base/Core/Sensor/include/Common/SensorVariable.h");
    addLocalIncludeFile(&varFile, "_CoLa_Extension.h");

    addBlankLine(&varFile);

    openNamespace(&varFile, "ssbl");
    openNamespace(&varFile,
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetSkeletonName());

    writeVariableClassDefinitions(&varFile);

    // end namespace and close file
    closeNamespace(&varFile,
                   static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                               ->GetSkeletonName());
    closeNamespace(&varFile, "ssbl");

    varFile << endl;
    varFile.close();
  }
 }

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableClassDefinitions(
      std::ofstream* pHeaderFile) {
  *pHeaderFile << "template <class T>" << endl;
  *pHeaderFile
      << "class "
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
      << "_Var";
  *pHeaderFile << " : public SensorVariable" << endl;
  *pHeaderFile << "{" << endl;
  *pHeaderFile << "public:" << endl;
  *pHeaderFile
      << "\t"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
      << "_Var(";
  *pHeaderFile << "\t\tstd::string name," << endl;
  *pHeaderFile << "\t\tstd::string comName," << endl;
  *pHeaderFile << "\t\tuint16_t idx," << endl;
  *pHeaderFile << "\t\tVariableDirection rwDir," << endl;
  *pHeaderFile << "\t\tAccessLevel readAccessLvl," << endl;
  *pHeaderFile << "\t\tAccessLevel writeAccessLvl," << endl;
  *pHeaderFile
      << "\t\tint32_t eventIdx) : SensorVariable(name, comName, idx, ";
  *pHeaderFile << "rwDir, readAccessLvl, writeAccessLvl, eventIdx) {};"
                << endl;
  *pHeaderFile
      << "\t~"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
      << "_Var";
  *pHeaderFile << "(){};" << endl;
  *pHeaderFile << "\tuint32_t SerializeContent(Serializer * pSer, uint8_t * "
                  "pDest, uint32_t * pOffset)"
                << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile << "\t\tswitch (pSer->GetProtocolType())" << endl;
  *pHeaderFile << "\t\t{" << endl;

  vector<std::string> done;

  std::vector<SensorInterfaceProtocol*> iFaceProtos =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetProtocols();

  for (vector<SensorInterfaceProtocol*>::iterator it = iFaceProtos.begin();
        it != iFaceProtos.end(); ++it) {
    if (std::find(done.begin(), done.end(), (*it)->GetProtocolType()) ==
        done.end()) {
      *pHeaderFile << "\t\tcase ";

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
      *pHeaderFile << ((*it)->GetProtocolType()) << ":" << endl;
      *pHeaderFile << "\t\t\t(reinterpret_cast<" +
                          static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetDeviceName() +
                          "_";
      *pHeaderFile << out;
      *pHeaderFile
          << "Serializer*> (pSer))->Serialize(pDest, this->Value_, pOffset);"
          << endl;
      *pHeaderFile << "\t\t\tbreak;" << endl;

      done.push_back((*it)->GetProtocolType());
    }
  }

  *pHeaderFile << "\t\tdefault:" << endl;
  *pHeaderFile << "\t\t\tbreak;" << endl;
  *pHeaderFile << "\t\t}" << endl;
  *pHeaderFile << "\t\treturn *pOffset;" << endl;
  *pHeaderFile << "\t}" << endl;

  done.clear();
  *pHeaderFile
      << "\tuint32_t DeserializeContent(Deserializer* pDes, uint8_t* pSrc)"
      << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile << "\t\tuint32_t offset=0;" << endl;
  *pHeaderFile << "\t\tswitch (pDes->GetProtocolType())" << endl;
  *pHeaderFile << "\t\t{" << endl;

  for (vector<SensorInterfaceProtocol*>::iterator it = iFaceProtos.begin();
        it != iFaceProtos.end(); ++it) {
    if (std::find(done.begin(), done.end(), (*it)->GetProtocolType()) ==
        done.end()) {
      *pHeaderFile << "\t\tcase ";

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
      *pHeaderFile << ((*it)->GetProtocolType()) << ":" << endl;
      *pHeaderFile << "\t\t\t(reinterpret_cast<" +
                          static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetDeviceName() +
                          "_";
      *pHeaderFile << out;
      *pHeaderFile << "Deserializer*> (pDes))->Deserialize(pSrc, "
                      "this->Value_, &offset);"
                    << endl;
      *pHeaderFile << "\t\t\tbreak;" << endl;

      done.push_back((*it)->GetProtocolType());
    }
  }

  *pHeaderFile << "\t\tdefault:" << endl;
  *pHeaderFile << "\t\t\tbreak;" << endl;
  *pHeaderFile << "\t\t}" << endl;
  *pHeaderFile << "\t\treturn offset;" << endl;
  *pHeaderFile << "\t}" << endl;

  done.clear();

  *pHeaderFile << "\tsize_t Size(void){return sizeof(T);};" << endl;

  *pHeaderFile << "\tSensorResult		GetRaw(uint8_t * pDest)"
                << endl;
  *pHeaderFile << "\t{" << endl;
  *pHeaderFile << "\t\tif(NULL == pDest)" << endl;
  *pHeaderFile << "\t\t\treturn SSBL_ERR_INVALID_ARGUMENT;" << endl;
  *pHeaderFile << "\t\tmemcpy(pDest, &this->Value_,sizeof(T) );" << endl;

  *pHeaderFile << "\t\treturn SSBL_SUCCESS;" << endl;
  *pHeaderFile << "\t}" << endl;

  *pHeaderFile << "\ttemplate <class C>" << endl;
  *pHeaderFile << "\tSensorResult CopyElement(C & src, C & dest) {" << endl;
  *pHeaderFile << "\t\tstd::memcpy(&dest, &src, sizeof(C));" << endl;
  *pHeaderFile << "\t\treturn SSBL_SUCCESS;" << endl;
  *pHeaderFile << "\t}" << endl;

  for (auto s : BaseTypes_) {
    *pHeaderFile
        << "\tvirtual SensorResult GetBasicElement(const std::string& "
            "elementName, ";
    *pHeaderFile << s << "& value) = 0;" << endl;
  }
  *pHeaderFile << "\tvirtual ComObj* Clone() const = 0;" << endl;

  *pHeaderFile << "\tT Value_;" << endl;
  *pHeaderFile << "};" << endl;
  *pHeaderFile << endl;

  vector<CoLaVariable*> vars =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetVariables();

  for (vector<CoLaVariable*>::iterator it = vars.begin(); it != vars.end();
        ++it) {
    writeVariableClassDefinition(pHeaderFile, (*it));
  }
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableClassDefinition(
    std::ofstream* pHeaderFile,
                                          CoLaVariable* pVar) {
  GenericCoLaTypeContainer* pT = pVar->getType();

  *pHeaderFile
      << "class " << pVar->getName() << "_"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
               << "_Var";
  *pHeaderFile
      << " : public "
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
      << "_Var<";
  *pHeaderFile << pT->GetBaseTypeName() << ">" << endl;

  *pHeaderFile << "{" << endl;
  *pHeaderFile << "public:" << endl;
  *pHeaderFile << "\t"
               << pVar->getName() + "_" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Var";

  *pHeaderFile << "();" << endl;
  *pHeaderFile << "\t~"
               << pVar->getName() + "_" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Var";

  *pHeaderFile << "(){};" << endl;

  *pHeaderFile << "\tComObj* Clone() const override { return new ";
  *pHeaderFile
      << pVar->getName() + "_" +
             static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
             "_Var(*this);}" << endl;



  *pHeaderFile << "\tstatic SensorVariable* Create() { return new " +
                      pVar->getName()
      << "_"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
             "_Var; }"
      << endl;

  writeVariableGetterSetterDefinition(pHeaderFile);

  *pHeaderFile << "};" << endl;

  *pHeaderFile << endl;
}


//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVariableGetterSetterDefinition(
    std::ofstream* pHeaderFile) {
  for (auto s : BaseTypes_) {
    *pHeaderFile << "\tSensorResult GetBasic(";
    *pHeaderFile << s << "& value) override;" << endl;
  }
  for (auto s : BaseTypes_) {
    *pHeaderFile
        << "\tSensorResult GetBasicElement(const std::string& elementName, ";
    *pHeaderFile << s << "& value) override;" << endl;
  }
  for (auto s : BaseTypes_) {
    *pHeaderFile << "\tSensorResult SetBasic(";
    *pHeaderFile << s << "& value) override;" << endl;
  }

  *pHeaderFile
      << "\tSensorResult SetBasicFromString(std::string value) override;"
      << endl;

  for (auto s : BaseTypes_) {
    *pHeaderFile
        << "\tSensorResult SetBasicElement(const std::string& elementName, ";
    *pHeaderFile << s << "& value)  override;" << endl;
  }
}