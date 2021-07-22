#pragma once
#include <string>
#include <vector>
#include "Device/include/Common/DeviceFamily.h"

using namespace std;

class Writer {
 public:
  Writer(const string& topDir);
  Writer(DeviceFamily* pDevFamily, const string& topDir);
  ~Writer(){};

  void WriteSkeletonLayer();
  virtual void WriteSkeletonLayerImpl(){};
  void writeLocalSkeletonFactoryDefinition(
      string& folder);
  void writeLocalSkeletonFactoryImplementation(
      string& folder);  

  void writeSkeletonSWIGFiles();
  void writeSkeletonSWIGFile(const string& dir);
  void writeSkeletonSWIGFileFinal(const string& dir);
  void writeSkeletonCMakeFiles();
  void writeSkeletonCMakeFile(const string& dir);
  void cleanFinalSkeletonCMakeFiles(const string& dir);
  void cleanSkeletonCMakeFiles(const string& dir);
  void writeFinalSkeletonCMakeFile(const string& dir); 
  void writeSsblTopInclude(vector<DeviceFamily*> devices);

 protected:



  void supressMsvcWarning(std::ofstream* pFile, uint32_t number);
  void addBlankLine(std::ofstream* pFile);
  void writeNoticeHeader(std::ofstream* pFile);
  void addSystemIncludeFile(std::ofstream* file, std::string systemheader);
  void addProjectIncludeFile(std::ofstream* file, std::string filename);
  void addLocalIncludeFile(std::ofstream* file, std::string suffix);
  void createSourceHeader(std::ofstream* file);
  void createHeaderHeader(std::ofstream* file);
  void openNamespace(std::ofstream* file, std::string Namespace);
  void closeNamespace(std::ofstream* file, std::string Namespace);
  void openClass(std::ofstream* file, std::string text);
  void closeClass(std::ofstream* file, std::string text);
  void openClassMethod(std::ofstream* file, std::string className,
                       std::string methodName);
  void closeClassMethod(std::ofstream* file);

  vector<string> BaseTypes_;
  string topDirectory_;
  DeviceFamily * pDevFamily_;
};