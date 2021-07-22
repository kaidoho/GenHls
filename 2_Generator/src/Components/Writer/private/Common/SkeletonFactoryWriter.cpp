#include <algorithm>
#include <filesystem>  
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/Common/Writer.h"
#include "Device/include/CoLa/CoLaDevice.h"
using namespace std::filesystem;

using namespace std;

void Writer::writeLocalSkeletonFactoryDefinition(
    std::string& folder) {
  ofstream  headerfile;

  string headerFileString =
      folder + "/include/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_Factory.h";
    
  headerfile.open(headerFileString.c_str(), ios::out | ios::trunc);

  createHeaderHeader(&headerfile);
  addProjectIncludeFile(&headerfile,
                        "Base/Core/Sensor/include/Common/SensorSkeleton.h");

  addBlankLine(&headerfile);
  openNamespace(&headerfile, "ssbl");
  openNamespace(
      &headerfile,
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
  headerfile << "SensorSkeleton * CreateSensorSkeleton(const std::string "
                "&SensorSkeletonName, const std::string &IP);"
             << endl;
  closeNamespace(
      &headerfile,
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
  closeNamespace(&headerfile, "ssbl");

}

void Writer::writeLocalSkeletonFactoryImplementation(string& folder) {

  ofstream sourcefile;
  string sourceFileString = folder + "/private/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_Factory.cpp";

  sourcefile.open(sourceFileString.c_str(), ios::out | ios::trunc);



  writeNoticeHeader(&sourcefile);
  addBlankLine(&sourcefile);
  string header =
      folder + "/include/" +
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
      "_Factory.h";
 
  size_t npos = header.find_first_of("API");
  header = header.substr(npos);
  addProjectIncludeFile( &sourcefile, header);

  header = folder + "/include/" +
           static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetDeviceName() +
           ".h";

  npos = header.find_first_of("API");
  header = header.substr(npos);
  addProjectIncludeFile(&sourcefile, header);
  
  addProjectIncludeFile(&sourcefile, "Base/Core/Common/include/Hash.h");
  addBlankLine(&sourcefile);

  supressMsvcWarning(&sourcefile, 4307);
  addBlankLine(&sourcefile);

  openNamespace(&sourcefile, "ssbl");
  openNamespace(
      &sourcefile,
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());

  addBlankLine(&sourcefile);

  sourcefile << "SensorSkeleton * CreateSensorSkeleton(const std::string "
                "&SensorSkeletonName, const std::string &IP)"
             << endl;
  sourcefile << "{" << endl;
  sourcefile << "\tSensorSkeleton * pRet = NULL;" << endl;
  sourcefile << "\tuint64_t test = hash_64_fnv1a(SensorSkeletonName.c_str(), "
                "SensorSkeletonName.size());"
             << endl;
  sourcefile << "\tswitch (test)" << endl;
  sourcefile << "\t{" << endl;
  
  for (auto dev : pDevFamily_->GetFamilyMembers()) {
    sourcefile << "\t\tcase hash_64_fnv1a_const(\"" + dev + "\"):" << endl;
    sourcefile << "\t\t\tpRet = new " + pDevFamily_->GetName() + "(IP);" << endl;
    
    sourcefile << "\t\t\tbreak;" << endl;
  }

  sourcefile << "\t\tdefault:" << endl;
  sourcefile << "\t\t\tbreak;" << endl;
  sourcefile << "\t}" << endl;

  sourcefile << "\treturn pRet;" << endl;
  sourcefile << "}" << endl;
  closeNamespace(
      &sourcefile,
      static_cast<CoLaDevice*>(pDevFamily_->pDevBase_)->GetSkeletonName());
  closeNamespace(&sourcefile, "ssbl");
}

