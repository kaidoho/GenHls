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
void CoLaSkeletonWriter::writeCoLaExtDefinitionFile(
    const string& includefolder) {
  ofstream headerfile;
  string headerFileString =
      includefolder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_CoLa_Extension.h";
  headerfile.open(headerFileString.c_str(), ios::out | ios::trunc);

  std::vector<SensorInterface*> iFaces =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetInterfaces();
  vector<std::string> done;

  std::vector<SensorInterfaceProtocol*> iFaceProtos =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetProtocols();

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
      writeColaExtensionHeader(includefolder,  out);
      done.push_back((*it)->GetProtocolType());
    }
  }
}


//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeColaExtensionHeader(const string& includefolder,
                                const string& ColaDialect) {
  ofstream headerfile;
  string headerFileString =
      includefolder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_CoLa_Extension.h";

  headerfile.open(headerFileString.c_str(), ios::out | ios::app);

  if (headerfile.is_open()) {
    addBlankLine(&headerfile);

    writeSerializerHeader(&headerfile, ColaDialect);
    writeDeserializerHeader(&headerfile, ColaDialect);

    addBlankLine(&headerfile);

    openNamespace(&headerfile, "ssbl");
    openNamespace(&headerfile,
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetSkeletonName());
    openClass(
        &headerfile,
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
            "_" + ColaDialect +
                               "Protocol : public " + ColaDialect + "Protocol");
    headerfile << "\t" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                          ->GetDeviceName() +
                      "_" + ColaDialect +
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
    headerfile << "\t~" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                          ->GetDeviceName() +
                      "_" + ColaDialect +
                      "Protocol(){};"
               << endl;
    headerfile << "\tprivate:" << endl;
    headerfile << "\t\t" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                          ->GetDeviceName() +
                      "_" + ColaDialect +
                      "Serializer Serializer_;"
               << endl;
    headerfile << "\t\t" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                          ->GetDeviceName() +
                      "_" + ColaDialect +
                      "Deserializer Deserializer_;"
               << endl;
    headerfile << "\t};" << endl;

    addBlankLine(&headerfile);
    closeNamespace(
        &headerfile,
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
    closeNamespace(&headerfile, "ssbl");
    addBlankLine(&headerfile);

    headerfile.close();
  }
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeSerializerHeader(std::ofstream* pHeaderFile,
                             const string& ColaDialect) {
  std::vector<int32_t> FlexStringLengths;
  std::vector<int32_t> FixStringLengths;
  addBlankLine(pHeaderFile);

  openNamespace(pHeaderFile, "ssbl");
  openNamespace(
      pHeaderFile,
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());

  openClass(
      pHeaderFile,
            static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
          "_" + ColaDialect +
                             "Serializer : public " + ColaDialect +
                             "Serializer");
  *pHeaderFile << "\t" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_" + ColaDialect +
                      "Serializer(){};"
               << endl;
  *pHeaderFile << "\t~" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_" + ColaDialect +
                      "Serializer(){};"
               << endl;

  addBlankLine(pHeaderFile);
  *pHeaderFile << "\tusing::ssbl::" + ColaDialect + "Serializer::Serialize;"
               << endl;
  addBlankLine(pHeaderFile);

  vector<GenericCoLaTypeContainer*> types =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetTypes();
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

  closeClass(pHeaderFile,
             static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
          "_" + ColaDialect + "Serializer");
  addBlankLine(pHeaderFile);
  closeNamespace(
      pHeaderFile,
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName());
  closeNamespace(pHeaderFile, "ssbl");
  addBlankLine(pHeaderFile);
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeDeserializerHeader(std::ofstream* pHeaderFile,
                               const string& ColaDialect) {
  std::vector<int32_t> FlexStringLengths;
  std::vector<int32_t> FixStringLengths;
  addBlankLine(pHeaderFile);

  openNamespace(pHeaderFile, "ssbl");
  openNamespace(
      pHeaderFile,
          static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());

  openClass(
      pHeaderFile,
            static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
          "_" + ColaDialect +
                             "Deserializer : public " + ColaDialect +
                             "Deserializer");
  *pHeaderFile << "\t" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_" + ColaDialect +
                      "Deserializer(){};"
               << endl;
  *pHeaderFile << "\t~" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_" + ColaDialect +
                      "Deserializer(){};"
               << endl;

  addBlankLine(pHeaderFile);

  // GetUniqueStringLengths(FixStringLengths, FlexStringLengths);

  *pHeaderFile << "\tusing::ssbl::" + ColaDialect + "Deserializer::Deserialize;"
               << endl;
  addBlankLine(pHeaderFile);

  vector<GenericCoLaTypeContainer*> types =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetTypes();
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



  closeClass(pHeaderFile,
             static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
          "_" + ColaDialect + "Deserializer");
  addBlankLine(pHeaderFile);
  closeNamespace(
      pHeaderFile,
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
  closeNamespace(pHeaderFile, "ssbl");
  addBlankLine(pHeaderFile);
}