#pragma once
#include <string>
#include <vector>
#include "Device/include/Common/DeviceFamily.h"
#include "Writer/include/Common/Writer.h"
#include "Device/include/CoLa/CoLaDevice.h"



class CoLaSkeletonWriter : public Writer {
 public:
  CoLaSkeletonWriter(const string& outputFolder);
  CoLaSkeletonWriter(DeviceFamily* pDevFamily, const string& outputFolder);
  ~CoLaSkeletonWriter(){};

  void WriteSkeletonLayerImpl();

 protected:
  // Write the skeleton data type definitions
  void writeTypeDefinitionsFile(const string& outputFolder);
  void writeDataType(ofstream* pHeaderFile, GenericCoLaTypeContainer* pType);

  // Write the variable definitions
  void writeVariableDefinitionsFile(const std::string& outputFolder);
  void writeVariableClassDefinitions(std::ofstream* pHeaderFile);
  void writeVariableClassDefinition(std::ofstream* pHeaderFile,
                                    CoLaVariable* pVar);
  void writeVariableGetterSetterDefinition(std::ofstream* pHeaderFile);

  // Write the function definitions
  void writeMethodClassDefinitionFile(const std::string& folder);
  void writeMethodClassDefinition(ofstream* pHeaderFile, CoLaMethod* pMeth);
  void writeMethodClassDefinitions(std::ofstream* pHeaderFile);

  // Write the skeleton definition
  void writeDeviceClassDefinition(ofstream* pHeaderFile);
  void writeDeviceClassDefinitionFile(const std::string& folder);

  // Write the CoLa extension file
  void writeCoLaExtDefinitionFile(const std::string& folder);
  void writeColaExtensionHeader(const std::string& includefolder,
                                const std::string& ColaDialect);
  void writeSerializerHeader(std::ofstream* pHeaderFile,
                             const std::string& ColaDialect);
  void writeDeserializerHeader(std::ofstream* pHeaderFile,
                               const std::string& ColaDialect);

  // Write the CoLa extension file
  void writeCoLaExtImplementationFile(const std::string& folder);
  void writeColaExtension(const std::string& sourcefolder,
                          const std::string& ColaDialect);
  void writeSerializerImpl(std::ofstream* pHeaderFile,
                           const std::string& ColaDialect);
  void writeDeserializerImpl(std::ofstream* pHeaderFile,
                             const std::string& ColaDialect);

  void writeVarSerializeMethod(
      ofstream* pHeaderFile, GenericCoLaTypeContainer* pType,
                               const std::string& toplevelName,
                               int32_t arrayDepth, bool lastArray,
                               const std::string& coladialect);

  void writeVarDeserializeMethod(
      ofstream* pHeaderFile, GenericCoLaTypeContainer* pType,
                                 const std::string& toplevelName,
                                 int32_t arrayDepth, bool lastArray,
                                 const std::string& coladialect);

  // Write the variable implementation
  void writeVariableImplementationFile(const std::string& folder);
  void writeVariableImplementations(ofstream* pSourceFile, CoLaVariable* pVar);
  void writeVariableGetterSetterImplementations(std::ofstream* pSourceFile,
                                                CoLaVariable* pVar);
  void writeVariableGetterImplementations(std::ofstream* pSourceFile,
                                          CoLaVariable* pVar,
                                          const std::string& baseTypeName,
                                          vector<string> addrs);
  void writeVariableSetterImplementations(
      std::ofstream* pSourceFile, CoLaVariable* pVar,
      const std::string& baseTypeName,
      vector<tuple<std::string, std::string, std::string>> addrs);
  void writeSimpleVariableGetterImplementations(std::ofstream* pSourceFile,
                                                CoLaVariable* pVar,
                                                const std::string& baseTypeName,
                                                vector<string> addrs);
  void writeSimpleVariableSetterImplementations(
      std::ofstream* pSourceFile, CoLaVariable* pVar,
      const std::string& baseTypeName,
      vector<tuple<std::string, std::string, std::string>> addrs);
  
  void WriteVariableSetterCaseImplementation( std::ofstream* pSourceFile,
                                              const std::string& caseName,
                                              const std::string& element,
                                              const std::string& min, 
                                              const std::string& max);

  void WriteVariableGetterCaseImplementation(std::ofstream* pSourceFile,
                                             const std::string& caseName,
                                             const std::string& element);

  void findElements(
      GenericCoLaTypeContainer* pType, const std::string& prefix,
      std::vector<tuple<std::string, std::string, std::string, std::string>>&
          addresses,
      uint32_t lvl, uint32_t arrayDepth, bool lastArray);

  // write device class implementation
  void writeDeviceClassImplementationFile(const std::string& folder);

  // write method class implementation
  void writeMethodClassImplementations(const std::string& folder);
  void writeMethodClassImplementation(std::ofstream* pHeaderFile,
                                      CoLaMethod* pMeth);


};