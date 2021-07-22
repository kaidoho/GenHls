#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/CoLa/CoLaSkeletonWriter.h"

using namespace std;
using namespace std::filesystem;
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
//===========================================================================
void CoLaSkeletonWriter::writeCoLaExtImplementationFile(
    const string& sourcefolder) {
  ofstream sourcefile;
  string sourceFileString =
      sourcefolder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_CoLa_Extension.cpp";

  sourcefile.open(sourceFileString.c_str(), ios::out | ios::trunc);

  std::vector<SensorInterface*> iFaces =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetInterfaces();
  vector<std::string> done;

  if (sourcefile.is_open()) {
    createSourceHeader(&sourcefile);
    addLocalIncludeFile(&sourcefile, string("_") + "CoLa_Extension.h");
  }


  std::vector<SensorInterfaceProtocol*> iFaceProtos =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetProtocols();



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
      writeColaExtension( sourcefolder, out);
      done.push_back((*it)->GetProtocolType());
    }
  }


}



//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeColaExtension(const string& sourcefolder,
                                const string& ColaDialect) {
  ofstream sourcefile, headerfile;
  string sourceFileString =
      sourcefolder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_CoLa_Extension.cpp";

  sourcefile.open(sourceFileString.c_str(), ios::out | ios::app);


  if (sourcefile.is_open()) {
    addBlankLine(&sourcefile);
    writeSerializerImpl(&sourcefile, ColaDialect);
    writeDeserializerImpl(&sourcefile, ColaDialect);
    sourcefile.close();
  }


}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeSerializerImpl(std::ofstream* pSourceFile,
                                 const string& ColaDialect) {
  std::vector<int32_t> FlexStringLengths;
  std::vector<int32_t> FixStringLengths;

  addBlankLine(pSourceFile);

  openNamespace(pSourceFile, "ssbl");
  openNamespace(
      pSourceFile,
          static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());

  string front =
      "\tvoid " +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_" + ColaDialect + "Serializer";

  string method;
  string x = "";

  vector<GenericCoLaTypeContainer*> types =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetTypes();
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

  addBlankLine(pSourceFile);
  closeNamespace(
      pSourceFile,
          static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
  closeNamespace(pSourceFile, "ssbl");
  addBlankLine(pSourceFile);
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeDeserializerImpl(std::ofstream* pSourceFile,
                                   const string& ColaDialect) {
  std::vector<int32_t> FlexStringLengths;
  std::vector<int32_t> FixStringLengths;

  addBlankLine(pSourceFile);

  openNamespace(pSourceFile, "ssbl");
  openNamespace(
      pSourceFile,
          static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());

  string front =
      "\tvoid " +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_" + ColaDialect + "Deserializer";

  string method;
  string x = "";

  vector<GenericCoLaTypeContainer*> types =
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetTypes();
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

  addBlankLine(pSourceFile);
  closeNamespace(
      pSourceFile,
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
  closeNamespace(pSourceFile, "ssbl");
  addBlankLine(pSourceFile);
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVarSerializeMethod(ofstream* pHeaderFile, 
  GenericCoLaTypeContainer* pType, const string& toplevelName, int32_t arrayDepth, bool lastArray, 
  const string& coladialect) {

  string CounterName = arrayDepthToCounter(arrayDepth);
  string indent = getIndent(arrayDepth);

  TypeClass tc = pType->GetTypeClass();

  string iToplevelName = toplevelName;

  switch (tc) {
    case STRUCT_CONTAINER: {
      vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();
      *pHeaderFile << endl;

      for (vector<GenericCoLaTypeContainer*>::iterator it =
               subTypeList.begin();
           it != subTypeList.end(); ++it) {
        lastArray = false;
        writeVarSerializeMethod(pHeaderFile, (*it), iToplevelName, arrayDepth,
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
                   << iToplevelName << pType->GetLocalName()
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
            iToplevelName + pType->GetLocalName() + "[" + CounterName + "].",
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
                     << iToplevelName << pType->GetLocalName() << " , pOffset);"
                     << endl;
      } else {
        CounterName = arrayDepthToCounter(arrayDepth - 1);

        
        iToplevelName.pop_back();
        *pHeaderFile << indent << prefix + "Serialize(pDest, rSrc."
                     << iToplevelName << ", pOffset);" << endl;
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
        *pHeaderFile << indent << "Serialize(pDest, rSrc." << iToplevelName
                     << pType->GetLocalName() << " , pOffset);" << endl;
      } else {
        CounterName = arrayDepthToCounter(arrayDepth - 1);
        iToplevelName.pop_back();
        *pHeaderFile << indent << "Serialize(pDest, rSrc." << iToplevelName
                     << ", pOffset);" << endl;
      }
      break;
    }
    default:
      cout << "UPS" << endl;
      break;
  }

}
//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeVarDeserializeMethod(
    ofstream* pHeaderFile, GenericCoLaTypeContainer* pType,
    const string& toplevelName, int32_t arrayDepth, bool lastArray,
    const string& coladialect) {
  string CounterName = arrayDepthToCounter(arrayDepth);
  string indent = getIndent(arrayDepth);
  string iToplevelName = toplevelName;
  TypeClass tc = pType->GetTypeClass();

  switch (tc) {
    case STRUCT_CONTAINER: {
      vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();
      *pHeaderFile << endl;

      for (vector<GenericCoLaTypeContainer*>::iterator it = subTypeList.begin();
           it != subTypeList.end(); ++it) {
        lastArray = false;
        writeVarDeserializeMethod(pHeaderFile, (*it), iToplevelName, arrayDepth,
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
                   << iToplevelName << pType->GetLocalName()
                   << "[" + CounterName + "],";
      *pHeaderFile << "pOffset);" << endl;
      *pHeaderFile << indent << "}" << endl;
      break;
    }
    case ARRAY_CONTAINER: {
      arrayDepth++;
      std::vector<GenericCoLaTypeContainer*> subTypeList = pType->GetChildren();

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
      for (vector<GenericCoLaTypeContainer*>::iterator it = subTypeList.begin();
           it != subTypeList.end(); ++it) {
        lastArray = true;
        writeVarDeserializeMethod(
            pHeaderFile, (*it),
            iToplevelName + pType->GetLocalName() + "[" + CounterName + "].",
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
                     << iToplevelName << pType->GetLocalName() << " , pOffset);"
                     << endl;
      } else {
        CounterName = arrayDepthToCounter(arrayDepth - 1);
        iToplevelName.pop_back();
        *pHeaderFile << indent << prefix + "Deserialize(pSrc, rDest."
                     << iToplevelName << ", pOffset);" << endl;
      }

      break;
    }
    case USER_TYPE_CONTAINER: {
      if (!lastArray) {
        *pHeaderFile << indent << "Deserialize(pSrc, rDest." << iToplevelName
                     << pType->GetLocalName() << " , pOffset);" << endl;
      } else {
        CounterName = arrayDepthToCounter(arrayDepth - 1);
        iToplevelName.pop_back();
        *pHeaderFile << indent << "Deserialize(pSrc, rDest." << iToplevelName
                     << ", pOffset);" << endl;
      }
      break;
    }
    default:
      cout << "UPS" << endl;
      break;
  }
}