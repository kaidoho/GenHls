#include <algorithm>
#include <filesystem>  
#include <fstream>
#include <iostream>
#include <string>
#include "Writer/include/Writer.h"
using namespace std::filesystem;

using namespace std;

void Writer::writeSkeletonFactoryFiles(std::vector<std::string> devices) {
  string includeFolder = outputFolder_ + "/API/Skeleton/include";
  string sourceFolder = outputFolder_ + "/API/Skeleton/private";

  // create_directory(path(includeFolder));
  // create_directory(path(sourceFolder));

  ofstream sourcefile, headerfile;
  string sourceFileString = sourceFolder + "/" + "SickSkeletonFactory.cpp";
  string headerFileString = includeFolder + "/" + "SickSkeletonFactory.h";
  sourcefile.open(sourceFileString.c_str(), ios::out | ios::trunc);
  headerfile.open(headerFileString.c_str(), ios::out | ios::trunc);

  createHeaderHeader(&headerfile);
  addProjectIncludeFile(&headerfile,
                        "Base/Core/Sensor/include/Common/SickSensorSkeleton.h");

  addBlankLine(&headerfile);
  openNamespace(&headerfile, "ssbl");
  headerfile << "SickSensorSkeleton * CreateSensorSkeleton(const std::string "
                "&SensorSkeletonName, const std::string &IP);"
             << endl;
  closeNamespace(&headerfile, "ssbl");

  writeNoticeHeader(&sourcefile);
  addBlankLine(&sourcefile);

  for (auto dev : devices) {
    addProjectIncludeFile(&sourcefile,
                          "API/Skeleton/include/" + dev + "/" + dev + ".h");
  }
  addProjectIncludeFile(&sourcefile,
                        "API/Skeleton/include/SickSkeletonFactory.h");
  addProjectIncludeFile(&sourcefile, "Base/Core/Common/include/Hash.h");
  addBlankLine(&sourcefile);

  supressMsvcWarning(&sourcefile, 4307);
  addBlankLine(&sourcefile);

  openNamespace(&sourcefile, "ssbl");
  addBlankLine(&sourcefile);

  sourcefile << "SickSensorSkeleton * CreateSensorSkeleton(const std::string "
                "&SensorSkeletonName, const std::string &IP)"
             << endl;
  sourcefile << "{" << endl;
  sourcefile << "\tSickSensorSkeleton * pRet = NULL;" << endl;
  sourcefile << "\tuint64_t test = hash_64_fnv1a(SensorSkeletonName.c_str(), "
                "SensorSkeletonName.size());"
             << endl;
  sourcefile << "\tswitch (test)" << endl;
  sourcefile << "\t{" << endl;

  for (auto dev : devices) {
    sourcefile << "\t\tcase hash_64_fnv1a_const(\"" + dev + "\"):" << endl;
    sourcefile << "\t\t\tpRet = new Dev" + dev + "::" + dev + "(IP);" << endl;
    sourcefile << "\t\t\tbreak;" << endl;
  }

  sourcefile << "\t\tdefault:" << endl;
  sourcefile << "\t\t\tbreak;" << endl;
  sourcefile << "\t}" << endl;

  sourcefile << "\treturn pRet;" << endl;
  sourcefile << "}" << endl;

  closeNamespace(&sourcefile, "ssbl");
}

void Writer::writeSsblTopInclude(const string& outputFolder,                        
                                 std::vector<DeviceFamily*> devFamilies) {
  ofstream headerfile;
  string headerFileString = outputFolder + "/include/" + "SSBL.h";

  headerfile.open(headerFileString.c_str(), ios::out | ios::trunc);
  createHeaderHeader(&headerfile);
  addProjectIncludeFile(&headerfile, "Base/Core/OS/include/Time.h");
  addProjectIncludeFile(&headerfile, "VersionInfo.h");
  
  /*
  addProjectIncludeFile(
      &headerfile, "API/Families/SickLidar2D/include/SickLidar2D_Factory.h");
  addProjectIncludeFile(&headerfile,
                        "API/Families/SickLidar2D/include/SickLidar2D.h");
  addProjectIncludeFile(&headerfile,
                        "API/Skeleton/include/SickSkeletonFactory.h");
  */
  for (auto family : devFamilies) {
    vector<Device*> devices = family->GetDevices();
    for (auto device : devices) {
      addProjectIncludeFile(&headerfile,
                            "API/Skeleton/" + family->GetGroup() + "/" 
                            + family->GetName() + "/" + device->GetDeviceName() 
                            + "/include/" + device->GetSkeletonName() + ".h");
    }
  }
}
