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
void CoLaSkeletonWriter::writeMethodClassDefinitionFile(const string& folder) {
  ofstream methodFile;
  string headerFileString =
      folder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_Functions.h";

  methodFile.open(headerFileString.c_str(), ios::out | ios::trunc);

  if (methodFile.is_open()) {
    writeNoticeHeader(&methodFile);
    addBlankLine(&methodFile);
    methodFile << "#pragma once" << endl;
    addSystemIncludeFile(&methodFile, "stdint.h");

    addLocalIncludeFile(&methodFile, "_Types.h");
    addLocalIncludeFile(&methodFile, "_CoLa_Extension.h");
    addProjectIncludeFile(
        &methodFile, "Base/Core/Sensor/include/Common/SensorFunction.h");

    addBlankLine(&methodFile);

    openNamespace(&methodFile, "ssbl");
    openNamespace(&methodFile,
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetSkeletonName());

    writeMethodClassDefinitions(&methodFile);

    // end namespace and close file
    closeNamespace(&methodFile,
                   static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                               ->GetSkeletonName());
    closeNamespace(&methodFile, "ssbl");

    addBlankLine(&methodFile);
    methodFile.close();
  }
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeMethodClassDefinition(ofstream* pHeaderFile,
                                        CoLaMethod* pMeth) {
  std::vector<string> classTypes = {"NANR", "ANR", "NAR", "AR"};
  int32_t index = 0;
  string classSpec;
  if (NULL != pMeth->GetArgType() && NULL != pMeth->GetReturnType()) {
    classSpec = "AR<" + (pMeth->GetArgType())->GetBaseTypeName() + "," +
                (pMeth->GetReturnType())->GetBaseTypeName() + ">";
  } else if (NULL == pMeth->GetArgType() && NULL != pMeth->GetReturnType()) {
    classSpec = "NAR<" + (pMeth->GetReturnType())->GetBaseTypeName() + ">";
  } else if (NULL != pMeth->GetArgType() && NULL == pMeth->GetReturnType()) {
    classSpec = "ANR<" + (pMeth->GetArgType())->GetBaseTypeName() + ">";
  } else {
    classSpec = "NANR";
  }

  *pHeaderFile << "class " << pMeth->getName() << "_"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
             "_Func";
  *pHeaderFile
      << " : public "
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
      << "_Func_"
               << classSpec << endl;
  *pHeaderFile << "{" << endl;
  *pHeaderFile << "public:" << endl;
  *pHeaderFile << "\t" << pMeth->getName() << "_"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
             "_Func";
  *pHeaderFile << "();" << endl;
  *pHeaderFile << "\t~" << pMeth->getName() << "_"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
             "_Func";
  *pHeaderFile << "(){};" << endl;

  *pHeaderFile << "\tComObj* Clone() const override { return new ";
  *pHeaderFile
      << pMeth->getName() + "_" +
             static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
             "_Func(*this);}"
      << endl;




               

  *pHeaderFile << "\tstatic SensorFunction* Create() { return new " +
                      pMeth->getName()
      << "_"
      << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
             "_Func; }"
      << endl;

  *pHeaderFile << "};" << endl;
  *pHeaderFile << endl;
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeMethodClassDefinitions(
    std::ofstream* pHeaderFile) {
  std::vector<string> classFlags = {"false,false", "true,false", "false,true",
                                    "true,true"};
  std::vector<string> classMembers = {
      "\n",
      "Args Args_;\n",
      "Return Return_;\n",
      "Args Args_;\n\tReturn Return_;\n",
  };
  std::vector<string> classTypes = {"NANR", "ANR", "NAR", "AR"};

  uint32_t i = 0;

  for (vector<string>::iterator it = classTypes.begin(); it != classTypes.end();
       ++it) {
    if (0 == (*it).compare("ANR")) {
      *pHeaderFile << "template <class Args>" << endl;
    }
    if (0 == (*it).compare("NAR")) {
      *pHeaderFile << "template <class Return>" << endl;
    }
    if (0 == (*it).compare("AR")) {
      *pHeaderFile << "template <class Args, class Return>" << endl;
    }

    *pHeaderFile
        << "class "
        << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
        << "_Func_" << *it;
    *pHeaderFile << " : public SensorFunction" << endl;
    *pHeaderFile << "{" << endl;
    *pHeaderFile << "public:" << endl;
    *pHeaderFile
        << "\t"
        << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
        << "_Func_" << *it << "(";
    *pHeaderFile << "\t\tstd::string name," << endl;
    *pHeaderFile << "\t\tstd::string comname," << endl;
    *pHeaderFile << "\t\tuint16_t idx," << endl;
    *pHeaderFile << "\t\tAccessLevel accessLevel)" << endl;
    *pHeaderFile
        << "\t\t\t: SensorFunction(name,comname, idx, accessLevel, ";
    *pHeaderFile << classFlags[i];
    *pHeaderFile << ") {};" << endl;
    *pHeaderFile
        << "\t~"
        << static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName()
        << "_Func_" << *it;
    *pHeaderFile << "(){};" << endl;
    
    *pHeaderFile << "\tvirtual ComObj* Clone() const = 0;" << endl;
    vector<std::string> done;

    std::vector<SensorInterfaceProtocol*> iFaceProtos =
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetProtocols();

    if ((0 == (*it).compare("AR")) || (0 == (*it).compare("ANR"))) {
      *pHeaderFile << "\tuint32_t SerializeContent(Serializer * pSer, uint8_t "
                      "* pDest, uint32_t * pOffset)"
                   << endl;
      *pHeaderFile << "\t{" << endl;
      *pHeaderFile << "\t\tswitch (pSer->GetProtocolType())" << endl;
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
          *pHeaderFile
              << "Serializer*> (pSer))->Serialize(pDest, Args_, pOffset);"
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
    } else {
      *pHeaderFile << "\tuint32_t SerializeContent(Serializer * pSer, uint8_t "
                      "* pDest, uint32_t * pOffset)"
                   << endl;
      *pHeaderFile << "\t{" << endl;
      *pHeaderFile << "\t\tSSBL_UNUSED(pSer);" << endl;
      *pHeaderFile << "\t\tSSBL_UNUSED(pDest);" << endl;
      *pHeaderFile << "\t return *pOffset;" << endl;
      *pHeaderFile << "\t}" << endl;
    }

    done.clear();
    if ((0 == (*it).compare("AR")) || (0 == (*it).compare("NAR"))) {
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
          *pHeaderFile
              << "Deserializer*> (pDes))->Deserialize(pSrc, Return_, &offset);"
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

    } else {
      *pHeaderFile
          << "\tuint32_t DeserializeContent(Deserializer* pDes, uint8_t* pSrc)"
          << endl;
      *pHeaderFile << "\t{" << endl;
      *pHeaderFile << "\t\tSSBL_UNUSED(pDes);" << endl;
      *pHeaderFile << "\t\tSSBL_UNUSED(pSrc);" << endl;
      *pHeaderFile << "\t\treturn 0;" << endl;
      *pHeaderFile << "\t}" << endl;
    }

    *pHeaderFile << "\t" << classMembers[i];

    *pHeaderFile << "};" << endl;
    *pHeaderFile << endl;
    i++;
  }

  vector<CoLaMethod*> mths =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetMethods();
  for (vector<CoLaMethod*>::iterator it = mths.begin(); it != mths.end();
       ++it) {

    std::cout << (*it)->getName() << std::endl;
    writeMethodClassDefinition(pHeaderFile, (*it));
  }
}