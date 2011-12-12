cmake_minimum_required(VERSION 2.8)

project(OpenNI)

# Options
# ---------
OPTION(CREATE_OF_BUILD "Create a openFrameworks build" ON)
OPTION(CREATE_TEST_BUILD "Create a build which you do not have to install to" OFF)
if(CREATE_TEST_BUILD) 
	add_definitions(-DCUSTOM_SAMPLE_XML_PATH="./../config/SamplesConfig.xml")
	add_definitions(-DXN_USE_CUSTOM_MODULES_FILE="./../config/modules.xml")

	# Set relative install paths (for now we only support the test build)
	set(LIB_INSTALL_PATH "lib/")
	set(BIN_INSTALL_PATH "bin/")
	set(INCLUDE_FILES_INSTALL_PATH "include/")
	set(CONFIG_INSTALL_PATH "config/")
	set(INSTALL_NAME_VALUE "@executable_path/../lib") # only for mac, the @executable_path to lib

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/Bin/Test/config/SamplesConfig.xml DESTINATION ./${CONFIG_INSTALL_PATH})
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/Bin/Test/config/modules.xml DESTINATION ./${CONFIG_INSTALL_PATH})
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/Bin/Test/config/GlobalDefaults.ini DESTINATION ./${CONFIG_INSTALL_PATH})
	set(CMAKE_SKIP_RPATH false)
	set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
elseif(CREATE_OF_BUILD)
	set(OF_OPENNI_DATA_DIR "../../../data/openni/") 
	#set(OF_OPENNI_DATA_DIR "../../../../../../../addons/ofxOpenNI/")
        add_definitions(-DCUSTOM_SAMPLE_XML_PATH="${OF_OPENNI_DATA_DIR}config/SamplesConfig.xml")
        add_definitions(-DXN_USE_CUSTOM_MODULES_FILE="${OF_OPENNI_DATA_DIR}config/modules.xml")

        # Set relative install paths (for now we only support the test build)
        set(LIB_INSTALL_PATH "openni/lib/")
        set(BIN_INSTALL_PATH "bin/")
        set(INCLUDE_FILES_INSTALL_PATH "include/")
        set(CONFIG_INSTALL_PATH "openni/config/")
        #set(INSTALL_NAME_VALUE "@executable_path/${OF_OPENNI_DATA_DIR}lib/osx") # only for mac, the @executable_path to lib
	set(INSTALL_NAME_VALUE "@executable_path/${OF_OPENNI_DATA_DIR}lib") # only for mac, the @executable_path to lib
        #install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/Bin/openFrameworks/config/SamplesConfig.xml DESTINATION ./${CONFIG_INSTALL_PATH})
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/Bin/openFrameworks/config/modules.xml DESTINATION ./${CONFIG_INSTALL_PATH})
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/Bin/openFrameworks/config/GlobalDefaults.ini DESTINATION ./${CONFIG_INSTALL_PATH})
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/Bin/openFrameworks/config/ofxopenni_config.xml DESTINATION ./${CONFIG_INSTALL_PATH})
	set(CMAKE_SKIP_RPATH false)
        set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
else()
	message(FATAL_ERROR "For now you can only create a test build")
endif()

# Find platform 
# -------------
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(OPENNI_PLATFORM_LINUX true)
elseif (WIN32)
	set(OPENNI_PLATFORM_WIN32 true)
elseif (APPLE)
	set(OPENNI_PLATFORM_APPLE true)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" ${CMAKE_MODULE_PATH})

# Some global variables
# ----------------------
set(TINYXML_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/External/TinyXml/")
set(OPENNI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include/")
set(OPENNI_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/Modules")
set(TINYXML_LIB_NAME TinyXml)
set(JPEG_LIB_NAME JPEG)
set(OPENNI_LIB_NAME OpenNI)
set(NIMCOMMON_LIB_NAME nimCommon)
set(NIMCODECS_LIB_NAME nimCodecs)
set(NIMMOCKNODES_LIB_NAME nimMockNodes)
set(NIMRECORDER_LIB_NAME nimRecorder)

# Find JPEG system lib
# ---------------------
find_package(JPEG)
if(NOT JPEG_FOUND)
	# when not found use the one in the externals folder, so we can compile!
	set(JPEG_FOUND true)
	set(JPEG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/External/LibJPEG/")
	add_subdirectory("./Source/External/LibJPEG/")
endif()

# Find USB lib
# -------------
if(OPENNI_PLATFORM_LINUX OR OPENNI_PLATFORM_APPLE) 
  find_package(libusb-1.0)
  if(NOT USB_LIB_FOUND)
	# When not found compoile using libusb in external dir.
	set(USB_LIB_FOUND true)
	if (OPENNI_PLATFORM_APPLE)
		set(USB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/External/LibUSB/Mac/")
		set(USB_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/Source/External/LibUSB/Mac/libusb-1.0.a")
		message("WITH USB")
	endif()
  endif()
endif()

if (NOT USB_LIB_FOUND OR NOT JPEG_FOUND)
	message(FATAL_ERROR "Could not find library dependencies")
endif()

# @todo find tinyxml!

# Use external libraries.
# -----------------------
add_subdirectory("./Source/External/TinyXml/") # @todo only when lib was not found 
add_subdirectory("./Source/OpenNI/")
add_subdirectory("./Source/Modules/")

# Add samples
# ---------------------------
add_subdirectory("./Samples/NiAudioSample") #Compiles and runs on Mac
add_subdirectory("./Samples/NiBackRecorder") #Compiles and runs on Mac
add_subdirectory("./Samples/NiConvertXToONI") #Compiles and runs on Mac
#add_subdirectory("./Samples/NiCRead") #Compiles and runs on Mac # gives error in linux
add_subdirectory("./Samples/NiRecordSynthetic") #Compiles and runs on Mac
add_subdirectory("./Samples/NiSampleModule") #Library compiles on Mac
add_subdirectory("./Samples/NiSimpleCreate") #Compiles and runs on Mac
add_subdirectory("./Samples/NiSimpleRead")  #Compiles and runs on Mac
add_subdirectory("./Samples/NiSimpleViewer") #Compiles and runs on Mac
add_subdirectory("./Samples/NiUserTracker") #Compiles and runs on Mac
add_subdirectory("./Samples/NiViewer") #Compiles and runs on Mac
add_subdirectory("./Samples/ofxOpenNI") 

# Utils
# ---------------------------
add_subdirectory("./Source/Utils/")


