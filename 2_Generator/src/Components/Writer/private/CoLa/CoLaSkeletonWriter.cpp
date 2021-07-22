#include "Writer/include/CoLa/CoLaSkeletonWriter.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>


using namespace std;
using namespace std::filesystem;

CoLaSkeletonWriter::CoLaSkeletonWriter(const std::string& outputFolder)
    : Writer(outputFolder) {}

CoLaSkeletonWriter::CoLaSkeletonWriter(DeviceFamily* pDevFamily,
                          const std::string& outputFolder)
    : Writer(pDevFamily,outputFolder) {

}

void CoLaSkeletonWriter::WriteSkeletonLayerImpl() { 
 string includeFolder = topDirectory_ + "/Components/API/Skeleton/" +
                         pDevFamily_->GetGroup() + "/" +
                         pDevFamily_->GetName() + "/" +
                         pDevFamily_->GetNameVersionCombined() + "/include";

  writeTypeDefinitionsFile(includeFolder); 
  writeVariableDefinitionsFile(includeFolder);
  writeMethodClassDefinitionFile(includeFolder);
  writeDeviceClassDefinitionFile(includeFolder);
  writeCoLaExtDefinitionFile(includeFolder);
  string sourceFolder = topDirectory_ + "/Components/API/Skeleton/" +
                         pDevFamily_->GetGroup() + "/" + pDevFamily_->GetName() +
                        "/" + pDevFamily_->GetNameVersionCombined() + "/private";

  writeVariableImplementationFile(sourceFolder);
  writeMethodClassImplementations(sourceFolder);
  writeDeviceClassImplementationFile(sourceFolder);
  
  writeCoLaExtImplementationFile(sourceFolder);
  string top = topDirectory_ + "/Components/API/Skeleton/" +
                         pDevFamily_->GetGroup() + "/" + pDevFamily_->GetName() + "/" +
               pDevFamily_->GetNameVersionCombined();

  writeLocalSkeletonFactoryDefinition(top);
  writeLocalSkeletonFactoryImplementation(top);
}
