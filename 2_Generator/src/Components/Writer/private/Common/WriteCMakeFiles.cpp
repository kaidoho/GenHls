#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include "Writer/include/Common/Writer.h"

using namespace std;
using namespace std::filesystem;

bool IsLastFolder(const string& dir) {
  for (auto& p : std::filesystem::directory_iterator(path(dir))) {
    if (p.is_directory()) {
      if ((string::npos != p.path().string().find("private")) ||
          (string::npos != p.path().string().find("include"))) {
        return true;
      }
    }
  }
  return false;
}

void Writer::writeFinalSkeletonCMakeFile(const string& dir) {
  string cmFileString;
  ofstream cmFile;
  cmFileString = dir + "/CMakeLists.txt";
  cmFile.open(cmFileString.c_str(), ios::out | ios::app);
  cmFile << "cmake_minimum_required(VERSION 3.10)" << endl;

  cmFile << "#Project suffix is set to folder name" << endl;
  cmFile << "get_filename_component(COMPONENT_SUFFIX ${CMAKE_CURRENT_LIST_DIR} "
            "NAME)"
         << endl;
  cmFile << "string(REPLACE \" \" \"_\" COMPONENT_SUFFIX ${COMPONENT_SUFFIX})"
         << endl;
  cmFile << "string(TOLOWER ${COMPONENT_SUFFIX} COMPONENT_SUFFIX)" << endl;

  cmFile << "set(COMPONENT_SUFFIX \"${COMPONENT_SUFFIX}_skeleton\")" << endl;

  addBlankLine(&cmFile);
  cmFile << "set(BASE_NAME \"ssbl\")" << endl;
  cmFile << "set(LOCAL_PROJECT_NAME \"${BASE_NAME}-${COMPONENT_SUFFIX}\")"
         << endl;
  addBlankLine(&cmFile);
  cmFile << "project(${LOCAL_PROJECT_NAME}" << endl;
  cmFile << "VERSION \"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\"" << endl;
  cmFile << "LANGUAGES CXX)" << endl;

  cmFile << "set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} \"${TOP_DIR}/cmake\")"
         << endl;
  cmFile << "include(LibraryTargetExtension)" << endl;
  addBlankLine(&cmFile);
  cmFile << "ComponentSources(" << endl;
  cmFile << "NAME ${COMPONENT_SUFFIX}" << endl;
  cmFile << "SOURCES" << endl;

  for (auto& p : std::filesystem::directory_iterator(path(dir + "/private"))) {
    if (!p.is_directory()) {
      string out = p.path().string();
      size_t found = out.find_last_of("/\\");
      out = out.substr(found + 1);
      cmFile << out << endl;
    }
  }

  cmFile << ")" << endl;
  addBlankLine(&cmFile);
  cmFile << "if (CMAKE_CXX_COMPILER_ID STREQUAL \"GNU\")" << endl;

  for (auto& p : std::filesystem::directory_iterator(path(dir + "/private"))) {
    if (!p.is_directory()) {
      string out = p.path().string();
      size_t found = out.find_last_of("/\\");
      out = out.substr(found + 1);
      cmFile << "set_source_files_properties(${PROJECT_SOURCE_DIR}/private/";
      cmFile << out;
      cmFile << " PROPERTIES COMPILE_FLAGS -Wno-type-limits)" << endl;
    }
  }
 
  cmFile << "endif()" << endl;

  addBlankLine(&cmFile);
  cmFile << "CreateLibraryTarget(" << endl;
  cmFile << "BASE_NAME" << endl;
  cmFile << "${BASE_NAME}" << endl;
  cmFile << "COMPONENT_NAME" << endl;
  cmFile << "${COMPONENT_SUFFIX}" << endl;
  cmFile << "COMPONENTS" << endl;
  cmFile << "${COMPONENT_SUFFIX}" << endl;
  cmFile << "VS_SOLUTION_NAME" << endl;
  size_t pos = dir.find_last_of("API");
  string out = dir.substr(pos + 2);
  std::replace(out.begin(), out.end(), '\\', '/');
  pos = out.find_last_of("/");
  out = out.substr(0, pos);
  cmFile << out << endl;
  cmFile << "DEPENDS" << endl;
  cmFile << "base)" << endl;

  addBlankLine(&cmFile);
  cmFile << "CreateSwigTargets(" << endl;
  cmFile << "BASE_NAME" << endl;
  cmFile << "${BASE_NAME}" << endl;
  cmFile << "COMPONENT_NAME" << endl;
  cmFile << "${COMPONENT_SUFFIX}" << endl;
  cmFile << "MODULE_SUFFIX" << endl;
  cmFile << "skeleton" << endl;
  cmFile << "VS_SOLUTION_FOLDER" << endl;
  pos = dir.find_last_of("API");
  out = dir.substr(pos + 2);
  std::replace(out.begin(), out.end(), '\\', '/');
  pos = out.find_last_of("/");
  out = out.substr(0, pos);
  cmFile << "SWIG/" << out << endl;
  cmFile << "DEPENDS" << endl;
  cmFile << "${BASE_NAME}::${COMPONENT_SUFFIX}::static)" << endl;

  cmFile.close();
}

void Writer::writeSkeletonCMakeFile(const string& dir) {
  string cmFileString;
  ofstream cmFile;

  cmFileString = dir + "/CMakeLists.txt";
  cmFile.open(cmFileString.c_str(), ios::out | ios::app);

  for (auto& p : std::filesystem::directory_iterator(path(dir))) {
    if (p.is_directory()) {
      string out = p.path().string();
      size_t found = out.find_last_of("/\\");
      out = out.substr(found + 1);

      cmFile << "add_subdirectory(" << out << ")" << endl;

      if (!IsLastFolder(p.path().string())) {
        writeSkeletonCMakeFile(p.path().string());
      } else {
        writeFinalSkeletonCMakeFile(p.path().string());
      }
    }
  }
  cmFile.close();
}

void Writer::cleanFinalSkeletonCMakeFiles(const string& dir) {
  string cmFileString;
  ofstream cmFile;

  cmFileString = dir + "/CMakeLists.txt";
  cmFile.open(cmFileString.c_str(), ios::out | ios::trunc);
  cmFile.close();
}

void Writer::cleanSkeletonCMakeFiles(const string& dir) {
  string cmFileString;
  ofstream cmFile;

  cmFileString = dir + "/CMakeLists.txt";
  cmFile.open(cmFileString.c_str(), ios::out | ios::trunc);
  cmFile.close();

  for (auto& p : std::filesystem::directory_iterator(path(dir))) {
    if (p.is_directory()) {
      if (!IsLastFolder(p.path().string())) {
        cleanSkeletonCMakeFiles(p.path().string());
      } else {
        cleanFinalSkeletonCMakeFiles(p.path().string());
      }
    }
  }
}

void Writer::writeSkeletonCMakeFiles() {
  string skeletonDir = topDirectory_ + "/Components/API/Skeleton";

  cleanSkeletonCMakeFiles(skeletonDir);

  writeSkeletonCMakeFile(skeletonDir);
}

void Writer::writeSkeletonSWIGFileFinal(const string& dir) {
  string cmFileString;
  ofstream cm;

  cmFileString = dir + "/swig-in.i.in";
  cm.open(cmFileString.c_str(), ios::out | ios::trunc);
  cm << "%module @SWIG_MODULE_NAME@" << endl;
  cm << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/Common/include/Assert.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/Common/include/CallbackProvider.h\""
     << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/Common/include/Endianess.h\""
     << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/Common/include/Hash.h\""
     << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/Common/include/MakeUnique.h\""
     << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/Driver/include/Socket.h\""
     << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/OS/include/Event.h\"" << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/OS/include/Mutex.h\"" << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/OS/include/Task.h\"" << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/OS/include/Time.h\"" << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Core/OS/include/Timer.h\"" << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/OS/include/VariableEventQueue.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/Sensor/include/CoLa/"
        "CoLaSensorSkeleton.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/Sensor/include/Common/"
        "SensorComObjRepository.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/Sensor/include/Common/"
        "SensorFunction.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/Sensor/include/Common/"
        "SensorInterface.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/Sensor/include/Common/"
        "SensorSkeleton.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/Sensor/include/Common/"
        "SensorVariable.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Core/Sensor/private/CoLa/"
        "DefaultCoLaFunctions.h\""
     << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Logger/include/Logger.h\"" << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/CoLa/CoLaADeserializer.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/CoLa/CoLaAProtocol.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/CoLa/CoLaASerializer.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/CoLa/CoLaBDeserializer.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/CoLa/CoLaBProtocol.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/CoLa/CoLaBSerializer.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/CoLa/CoLaProtocol.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/Common/Checksum.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/Common/ComObject.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/Common/Deserializer.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/Common/Protocol.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/Common/Serializer.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Protocol/include/Common/TelegramBuffer.h\""
     << endl;
  cm << "%import \"@COMPONENTS_TOP_DIR@Base/Types/include/BasicTypes.h\""
     << endl;
  cm << "%import "
        "\"@COMPONENTS_TOP_DIR@Base/Types/include/SickSensorReturnCodes.h\""
     << endl;

  cm << "%import \"@PROJECT_SOURCE_DIR@/include/@FAMILY_NAME@_Types.h\"" << endl;
  cm << "%import \"@PROJECT_SOURCE_DIR@/include/@FAMILY_NAME@_Variables.h\""
     << endl;
  cm << "%import \"@PROJECT_SOURCE_DIR@/include/@FAMILY_NAME@_Functions.h\""
     << endl;
  cm << "%import \"@PROJECT_SOURCE_DIR@/include/@FAMILY_NAME@_Factory.h\""
     << endl;
  cm << endl;
  cm << "%{" << endl;
  cm << "#include \"@PROJECT_SOURCE_DIR@/include/@FAMILY_NAME@.h\""  << endl;
  cm << "%}" << endl;
  cm << endl;

  cm << "%include \"@PROJECT_SOURCE_DIR@/include/@FAMILY_NAME@.h\"" << endl;
  cm.close();

}

void Writer::writeSkeletonSWIGFile(const string& dir) {
  for (auto& p : std::filesystem::directory_iterator(path(dir))) {
    if (p.is_directory()) {
      if (IsLastFolder(p.path().string())) {
        string iFolder = p.path().string() + "/interface";
        if (!exists(path(iFolder))) create_directory(path(iFolder));
        writeSkeletonSWIGFileFinal(iFolder);
      } else {
        writeSkeletonSWIGFile(p.path().string());
      }
    }
  }
}

void Writer::writeSkeletonSWIGFiles() {
  string skeletonDir = topDirectory_ + "/Components/API/Skeleton";

  writeSkeletonSWIGFile(skeletonDir);
}
