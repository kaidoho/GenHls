#include "Writer/include/Common/Writer.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>


using namespace std;
using namespace std::filesystem;

Writer::Writer(const std::string& topDir) : topDirectory_(topDir) {
  BaseTypes_.push_back("int8_t");
  BaseTypes_.push_back("int16_t");
  BaseTypes_.push_back("int32_t");
  BaseTypes_.push_back("int64_t");
  BaseTypes_.push_back("uint8_t");
  BaseTypes_.push_back("uint16_t");
  BaseTypes_.push_back("uint32_t");
  BaseTypes_.push_back("uint64_t");
  BaseTypes_.push_back("float");
  BaseTypes_.push_back("double");
}

Writer::Writer(DeviceFamily* pDevFamily, const std::string& topDir)
    : pDevFamily_(pDevFamily), topDirectory_(topDir) {
  BaseTypes_.push_back("int8_t");
  BaseTypes_.push_back("int16_t");
  BaseTypes_.push_back("int32_t");
  BaseTypes_.push_back("int64_t");
  BaseTypes_.push_back("uint8_t");
  BaseTypes_.push_back("uint16_t");
  BaseTypes_.push_back("uint32_t");
  BaseTypes_.push_back("uint64_t");
  BaseTypes_.push_back("float");
  BaseTypes_.push_back("double");
}



void Writer::WriteSkeletonLayer() {

  string skeltonFolder = topDirectory_ + "/Components/API/Skeleton";
  string groupFolder = skeltonFolder + "/" + pDevFamily_->GetGroup(); 
  string familyFolder =
      groupFolder + "/" + pDevFamily_->GetName();
  string deviceFolder = familyFolder + "/" +
                        pDevFamily_->GetNameVersionCombined();
  string includeFolder =
      deviceFolder +  "/include";
  string sourceFolder =
      deviceFolder +  "/private";


  if (!exists(path(groupFolder)))   create_directory(path(groupFolder));
  if (!exists(path(familyFolder)))  create_directory(path(familyFolder));
  if (!exists(path(deviceFolder))) create_directory(path(deviceFolder));
  if (!exists(path(includeFolder))) create_directory(path(includeFolder));
  if (!exists(path(sourceFolder)))  create_directory(path(sourceFolder));
  WriteSkeletonLayerImpl();

  

}

void Writer::writeSsblTopInclude(vector<DeviceFamily*> devices) {
  ofstream headerfile;
  string headerFileString = topDirectory_ + "/include/" + "SSBL.h";
  vector<string> Families;


  headerfile.open(headerFileString.c_str(), ios::out | ios::trunc);
  createHeaderHeader(&headerfile);

  addProjectIncludeFile(&headerfile, "Base/Core/OS/include/Time.h");

  for (auto dev : devices) {
    Families.push_back(dev->GetGroup());
  }
  for (auto family : Families) {

    string inc = "API/Family/" + family + "/Common/include/" + family + ".h";
    addProjectIncludeFile(&headerfile, inc);

  }


  for (auto dev : devices) {
    Families.push_back(dev->GetGroup());

    addProjectIncludeFile(
        &headerfile, "API/Skeleton/" + dev->GetGroup() + "/" + dev->GetName() + "/" +
                                           dev->GetNameVersionCombined() + 
                         "/include/" + dev->GetName() + ".h");
      
  }
}