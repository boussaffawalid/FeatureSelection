PROJECT(FeatureSelection)


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


#include OTB library
FIND_PACKAGE(OTB)
IF(OTB_FOUND)  
  INCLUDE(${OTB_USE_FILE})  
ELSE(OTB_FOUND)  
  MESSAGE(FATAL_ERROR  
      "Cannot build OTB project without OTB.  Please set OTB_DIR.")  
ENDIF(OTB_FOUND)


#-----------------------------------------------------------------------------
# Installation vars.
# OTB_INSTALL_BIN_DIR          - binary dir (executables)
# OTB_INSTALL_LIB_DIR          - library dir (libs)


Message("OTB_DIR= ${OTB_DIR}")

get_filename_component(P_DIR ${OTB_DIR} PATH)
get_filename_component(PP_DIR ${P_DIR} PATH)

set(OTB_INSTALL_APP_DIR ${OTB_DIR}/applications )
set(OTB_INSTALL_BIN_DIR ${PP_DIR}/bin )
set(LIB_TYPE SHARED )



Message("OTB_INSTALL_APP_DIR= ${OTB_INSTALL_APP_DIR}")
Message("OTB_INSTALL_BIN_DIR= ${OTB_INSTALL_BIN_DIR}")



#include directory
include_directories(include)


########################################################################
# FST
########################################################################
set(OTB_USE_FST true)
IF(OTB_USE_FST)
	#include_directories(/usr/include/boost)
	set(FST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/FST3lib/_src_criteria
					${CMAKE_CURRENT_SOURCE_DIR}/extern/FST3lib/_src_dataio
					${CMAKE_CURRENT_SOURCE_DIR}/extern/FST3lib/_src_global
					${CMAKE_CURRENT_SOURCE_DIR}/extern/FST3lib/_src_search)
	include_directories(${FST_INCLUDE_DIR})
ENDIF()

#-----------------------------------------------------------------------------
# Configure files with settings for use by the build
# This must be done after the definition of all options
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/CMake/myOTBConfigure.h.in
               ${PROJECT_BINARY_DIR}/myOTBConfigure.h)
include_directories(${PROJECT_BINARY_DIR})


#add modules subdirectory
add_subdirectory(src)




