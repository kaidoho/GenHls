#include "Writer/include/CoLa/CoLaSkeletonWriter.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>


using namespace std;
using namespace std::filesystem;

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeMethodClassImplementations(const string& folder) {
  ofstream file;
  string FileString =
      folder + "/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_Functions.cpp";

  file.open(FileString.c_str(), ios::out | ios::trunc);

  if (file.is_open()) {
    writeNoticeHeader(&file);
    addBlankLine(&file);
    addLocalIncludeFile(&file, "_Functions.h");
    addLocalIncludeFile(&file, "_Variables.h");

    addBlankLine(&file);

    openNamespace(&file, "ssbl");
    openNamespace(&file,
                  static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                              ->GetSkeletonName());

    vector<CoLaMethod*> meth =
        static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetMethods();
    for (vector<CoLaMethod*>::iterator it = meth.begin(); it != meth.end();
         ++it) {
      writeMethodClassImplementation(&file, (*it));
    }
    // end namespace and close file
    closeNamespace(&file,
                   static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)
                               ->GetSkeletonName());
    closeNamespace(&file, "ssbl");

    file.close();
  }
}

//===========================================================================
//===========================================================================
void CoLaSkeletonWriter::writeMethodClassImplementation(
    std::ofstream* pHeaderFile,
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

  *pHeaderFile << pMeth->getName() + "_" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Func::";
  *pHeaderFile << pMeth->getName() + "_" +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Func()"
               << std::endl;

  *pHeaderFile << "\t: " +
                      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
                      "_Func_" + classSpec
               << std::endl;

  *pHeaderFile << "{" << std::endl;
  *pHeaderFile << "}" << std::endl;
  addBlankLine(pHeaderFile);
}

