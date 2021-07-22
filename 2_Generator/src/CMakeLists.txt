include(GeneratorExtension)

include_directories(${PROJECT_SOURCE_DIR}/src/Components)


ComponentSources(
  NAME Main
  PATH Main
  SOURCES 
  Main.cpp
)


ComponentSources(
  NAME Parser
  PATH Components/Parser
  SOURCES 
  CoLa/CoLaDeviceParser.cpp
  Common/Parser.cpp
  Common/ParserFactory.cpp
)

ComponentSources(
  NAME Logger
  PATH Components/Logger
  SOURCES 
  Logger.cpp
)


ComponentSources(
  NAME TinyXML2
  PATH Components/TinyXML2
  SOURCES 
  tinyxml2.cpp
)

ComponentSources(
  NAME Writer
  PATH Components/Writer
  SOURCES 
  Common/Writer.cpp
  Common/SimplePrinterFunctions.cpp
  Common/SkeletonFactoryWriter.cpp
  Common/WriteCMakeFiles.cpp
  CoLa/CoLaSkeletonWriter.cpp
  CoLa/header/CoLaSkeletonWriteTypeDefs.cpp
  CoLa/header/CoLaSkeletonWriteVarDefs.cpp
  CoLa/header/CoLaSkeletonWriteFuncDefs.cpp
  CoLa/header/CoLaSkeletonWriteDevDef.cpp
  CoLa/header/CoLaSkeletonWriteCoLaExtDef.cpp
  CoLa/source/CoLaSkeletonWriteVarImpl.cpp
  CoLa/source/CoLaSkeletonWriteFuncImpl.cpp
  CoLa/source/CoLaSkeletonWriteDevImpl.cpp
  CoLa/source/CoLaSkeletonWriteCoLaExtImpl.cpp
  
)

ComponentSources(
  NAME Device
  PATH Components/Device
  SOURCES 
  Common/Device.cpp
  CoLa/CoLaDevice.cpp
  CoLa/CoLaMethod.cpp
  CoLa/CoLaVariable.cpp
)



CreateExecutableTarget(
BASE_NAME
  Generator
COMPONENTS
  Main
  Parser
  TinyXML2
  Writer
  Device
  Logger
)


