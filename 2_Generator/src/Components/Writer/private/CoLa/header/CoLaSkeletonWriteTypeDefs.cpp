#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/CoLa/CoLaSkeletonWriter.h"

using namespace std;
using namespace std::filesystem;

void CoLaSkeletonWriter::writeTypeDefinitionsFile(const string& outputFolder) {
  ofstream file;
  string headerFileString = outputFolder + "/" +
                            static_cast<CoLaDevice*>( pDevFamily_->pDevBase_)->GetDeviceName() +
                            "_Types.h";

  file.open(headerFileString.c_str(), ios::out | ios::trunc);

  if (file.is_open()) {
    createHeaderHeader(&file);

    addSystemIncludeFile(&file, "stdint.h");
    addProjectIncludeFile(&file, "Base/Types/include/BasicTypes.h");

    addBlankLine(&file);
    openNamespace(&file, "ssbl");
    openNamespace(&file, static_cast<CoLaDevice*>( pDevFamily_->pDevBase_ )->GetSkeletonName());

    //  writeEnumBaseTypes(&file);

    //  writeStringBaseTypes(&file);
    vector<GenericCoLaTypeContainer*> types =
        static_cast<CoLaDevice*>( pDevFamily_->pDevBase_)->GetTypes();
    for (vector<GenericCoLaTypeContainer*>::iterator it = types.begin();
         it != types.end(); ++it) {
      writeDataType(&file, (*it));
    }

    addBlankLine(&file);
    closeNamespace(&file,
                   static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
    closeNamespace(&file, "ssbl");
    addBlankLine(&file);

    file.close();
  }

}

void CoLaSkeletonWriter::writeDataType(ofstream* pHeaderFile,
                           GenericCoLaTypeContainer* pType) {
  TypeClass tc = pType->GetTypeClass();
  StructContainer* pX = dynamic_cast<StructContainer*>(pType);

  switch (tc) {
    case STRUCT_CONTAINER: {
      *pHeaderFile << "#pragma pack(push, 1)" << endl;
      *pHeaderFile << "typedef struct " << endl;
      *pHeaderFile << "{" << endl;
      std::vector<GenericCoLaTypeContainer*> subTypeList =
          pType->GetChildren();
      for (vector<GenericCoLaTypeContainer*>::iterator it =
               subTypeList.begin();
           it != subTypeList.end(); ++it) {
        writeDataType(pHeaderFile, (*it));
      }

      *pHeaderFile << "} " << pType->GetBaseTypeName() << ";" << endl;
      *pHeaderFile << "#pragma pack(pop)" << endl;
      *pHeaderFile << endl;

      break;
    }
    case ARRAY_CONTAINER:

      if (dynamic_cast<ArrayContainer*>(pType)->IsFlexArray()) {
        *pHeaderFile << "\tuint16_t uiLength" + pType->GetLocalName() + ";"
                     << endl;
      }
      *pHeaderFile << "\t" << pType->GetBaseTypeName() << "\t"
                   << pType->GetLocalName() << "["
                   << to_string(
                          dynamic_cast<ArrayContainer*>(pType)->GetArraySize())
                   << "];" << endl;

      break;
    case USER_TYPE_CONTAINER:
      *pHeaderFile << "\t" << pType->GetBaseTypeName() << "\t"
                   << pType->GetLocalName() << ";" << endl;
      break;
    case CONCRETE_TYPE_CONTAINER: {
      *pHeaderFile << "\t" << pType->GetBaseTypeName() << "\t"
                   << pType->GetLocalName() << ";" << endl;
      break;
    }
    case XBYTE_CONTAINER: {
      *pHeaderFile
          << "\tuint8_t "
          << "\t" << pType->GetLocalName() << "["
          << to_string(dynamic_cast<XByteContainer*>(pType)->GetWidthInBytes())
          << "];" << endl;
      break;
    }
    case ENUM_CONTAINER: {
      *pHeaderFile << "\t" << pType->GetBaseTypeName() << "\t"
                   << pType->GetLocalName() << ";" << endl;
      break;
    }
    case STRING_CONTAINER: {
      *pHeaderFile << "#pragma pack(push, 1)" << endl;
      *pHeaderFile << "typedef struct " << endl;
      *pHeaderFile << "{" << endl;

      if (!(dynamic_cast<StringContainer*>(pType)->isFixLength())) {
        *pHeaderFile << "\t uint16_t uiLength;" << endl;
      }

      *pHeaderFile << "\tuint8_t Text["
                   << to_string(
                          dynamic_cast<StringContainer*>(pType)->GetLength())
                   << "];" << endl;
      *pHeaderFile << "} " << pType->GetBaseTypeName() << ";" << endl;
      *pHeaderFile << "#pragma pack(pop)" << endl;
      *pHeaderFile << endl;
      break;
    }
    default:

      cout << "unknown type class" << endl;
      break;
  }
}
      
