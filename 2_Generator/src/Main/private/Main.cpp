
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>

#include "Logger/include/Logger.h"
#include "Parser/include/Common/Parser.h"
#include "Writer/include/CoLa/CoLaSkeletonWriter.h"

using namespace std::filesystem;
using namespace std;
using namespace tinyxml2;

int main(int argc, char** argv) {
  XMLError eResult = XML_SUCCESS;
  path sourcePath = "../../../../../src/CID-Tools/90_cleaned_cids";
  path destPath = "../../../../../src/Library/Components";
  path topPath = "../../../../../src/Library";
  sourcePath = absolute(sourcePath);
  destPath = absolute(destPath);

  std::vector<DeviceFamily*> devFamilies;
  std::vector<path> sourceFiles;


  // does source folder exist?
  if (exists(sourcePath)) {
    // does output folder exist?
    if (exists(destPath)) {

      // find all device descriptions (*.xml)
      for (auto& p : recursive_directory_iterator(sourcePath)) {
        if (is_regular_file(p.path()))
        {
          if (0 == p.path().extension().compare(".xml")) {
            sourceFiles.push_back(p.path());
          }   
        }
      }

      for (auto& sourceFile : sourceFiles) {
        DeviceFamilyParser DeviceFamilyParser(sourceFile.string());
        DeviceFamily* pDevFamily = new DeviceFamily();
        eResult = DeviceFamilyParser.Run(&pDevFamily);
        if (eResult == XML_SUCCESS) {
          Parser* Parser = createTypeParser(pDevFamily, sourceFile.string());
          if (nullptr != Parser) {
            // Parse the file
            if (XML_SUCCESS == Parser->Run(&pDevFamily)) {
              CoLaSkeletonWriter CoLaSkeletonWriter(pDevFamily,
                                                    topPath.string());
              CoLaSkeletonWriter.WriteSkeletonLayer();
            }
            delete Parser;
          }
          devFamilies.push_back(pDevFamily);

        } else {
          delete pDevFamily;
          return -1;
        }

      }
    
      Writer w = Writer(topPath.string());
      w.writeSkeletonCMakeFiles();
      w.writeSkeletonSWIGFiles();
      w.writeSsblTopInclude(devFamilies);
     
    } else {
      LOG_ERROR("Error: Destination path %s does not exist.",
                destPath.string().c_str());
      return -1;
    }
  } else {
    LOG_ERROR("Error: Source path %s does not exist.",
              sourcePath.string().c_str());
    return -1;
  }

  return 0;
}