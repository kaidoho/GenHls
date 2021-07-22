#######################################################################################################
# Boilerplate for CMake
#######################################################################################################

include(CMakeParseArguments)
include(CMakePackageConfigHelpers)



#######################################################################################################
# ComponentSources(
#   NAME componentName
#   PATH componentPath
#   SOURCES [sourcePath1, sourcePath2, ...]
# )
#
#######################################################################################################
function(ComponentSources)
  set(options)
  set(oneValueArgs NAME PATH)
  set(multiValueArgs SOURCES)
  cmake_parse_arguments(PARSED "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  if(NOT PARSED_NAME)
    message(FATAL_ERROR "ComponentSources: NAME must be set to the name of the component (should be unique in the given CMakeLists.txt)")
  endif()
  
  if(NOT PARSED_SOURCES)
    message(FATAL_ERROR "ComponentSources: SOURCES must contain the list of source file paths relative to PATH")
  endif()
  
  if(PARSED_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown arguments given to ComponentSources(): \"${PARSED_UNPARSED_ARGUMENTS}\"")
  endif()

  # Build list of source files
  foreach(SourceFilePath ${PARSED_SOURCES})
    if(NOT PARSED_PATH)
      list(APPEND Sources "private/${SourceFilePath}")
    else()
      list(APPEND Sources ${PARSED_PATH}/private/${SourceFilePath})
    endif()
  endforeach()

  # Set variable with source files in parent scope
  set(COMPONENT_SOURCES_${PARSED_NAME} ${Sources} PARENT_SCOPE)
  
  # Gather header files recursively
  if(NOT PARSED_PATH)
    get_filename_component(BasePathAbsolute ${CMAKE_CURRENT_LIST_DIR} REALPATH)
    
    file(GLOB_RECURSE HeadersTmp RELATIVE ${BasePathAbsolute} ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
    foreach(HeaderFilePath ${HeadersTmp})
      list(APPEND Headers ${HeaderFilePath})
    endforeach()
  else()
    get_filename_component(BasePathAbsolute ${PARSED_PATH} REALPATH)
    file(GLOB_RECURSE HeadersTmp RELATIVE ${BasePathAbsolute} ${PARSED_PATH}/*.h ${PARSED_PATH}/*.hpp)
    foreach(HeaderFilePath ${HeadersTmp})
      list(APPEND Headers ${PARSED_PATH}/${HeaderFilePath})
    endforeach()
  endif()
  
  # Set variable with header files in parent scope
  set(COMPONENT_HEADERS_${PARSED_NAME} ${Headers} PARENT_SCOPE)
endfunction()

#######################################################################################################
# CreateSourceGroupsVS(
#   CATEGORY componentPath
#   SOURCES [sourcePath1, sourcePath2, ...]
# )
#
# Helper to create a source group within Visual Studio IDE
#######################################################################################################
function(CreateSourceGroupsVS)
  set(options)
  set(oneValueArgs CATEGORY)
  set(multiValueArgs SOURCES)
  cmake_parse_arguments(PARSED "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  if(NOT PARSED_SOURCES)
    message(FATAL_ERROR "CreateSourceGroupsVS: SOURCES must be set to the list of sources to create groups from")
  endif()
  
  if(NOT PARSED_CATEGORY)
    set(CATEGORY "src")
  else()
    set(CATEGORY ${PARSED_CATEGORY})
  endif()

  foreach(SourceItem ${PARSED_SOURCES})
    get_filename_component(SourceGroupName ${SourceItem} DIRECTORY)
    if(PARSED_GROUP_PREFIX)
      set(SourceGroupName ${CATEGORY}/${PARSED_GROUP_PREFIX}/${SourceGroupName})
    else()
      set(SourceGroupName ${CATEGORY}/${SourceGroupName})
    endif()
    if(MSVC)
      string(REPLACE "/" "\\" SourceGroupName ${SourceGroupName})
    endif()
    if(PARSED_PATH)
      set(SourceItemAbsolute ${PARSED_PATH}/${SourceItem})
    else()
      
      set(SourceItemAbsolute ${SourceItem})
    endif()
    source_group(${SourceGroupName} FILES ${SourceItemAbsolute})
  endforeach()
endfunction()




#######################################################################################################
#######################################################################################################
function(CreateExecutableTarget)
  set(options)
  set(oneValueArgs    BASE_NAME)
  set(multiValueArgs  COMPONENTS)
  cmake_parse_arguments(PARSED "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT PARSED_BASE_NAME)
    message(FATAL_ERROR "CreateExecutableTarget: PARSED_BASE_NAME must contain the base name of the library")
  endif()  
  
  if(NOT PARSED_COMPONENTS)
    message(FATAL_ERROR "CreateExecutableTarget: PARSED_COMPONENTS must not be empty")
  endif()

 
  foreach(ComponentName ${PARSED_COMPONENTS})
    set(COMPONENT_SOURCES_VARIABLE COMPONENT_SOURCES_${ComponentName})
    set(COMPONENT_HEADERS_VARIABLE COMPONENT_HEADERS_${ComponentName})
    list(APPEND SourceListInternal "${${COMPONENT_SOURCES_VARIABLE}}" "${${COMPONENT_HEADERS_VARIABLE}}")
  endforeach()
  
  # Create source groups for Visual Studio
  if(MSVC)
    if(SourceListInternal)
      CreateSourceGroupsVS(SOURCES ${SourceListInternal})
    endif()
  endif()

  # Add headers from the include-folder to source groups
  get_filename_component(ComponentPathAbsolute "${PROJECT_SOURCE_DIR}/include" REALPATH)
  file(GLOB_RECURSE ComponentHeadersAbs RELATIVE "${PROJECT_SOURCE_DIR}" "${ComponentPathAbsolute}/*.h" "${ComponentPathAbsolute}/*.hpp")
  file(GLOB_RECURSE ComponentHeadersList RELATIVE "${ComponentPathAbsolute}" "${ComponentPathAbsolute}/*.h" "${ComponentPathAbsolute}/*.hpp")
  if(MSVC)  
    if(ComponentHeadersList)
      CreateSourceGroupsVS(SOURCES ${ComponentHeadersList} CATEGORY "include")
    endif()
    list(APPEND SourceListInternal ${ComponentHeadersAbs})
  endif()

  
  add_executable(${PARSED_BASE_NAME} ${SourceListInternal})

  set_property(TARGET ${PARSED_BASE_NAME} PROPERTY CXX_STANDARD 17)

endfunction()






