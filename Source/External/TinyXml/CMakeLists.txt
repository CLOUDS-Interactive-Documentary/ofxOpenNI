set(${TINYXML_LIB_NAME}_sources tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp)

add_library(${TINYXML_LIB_NAME} SHARED ${${TINYXML_LIB_NAME}_sources})

# The install_name is Mac OSX specific and is used to set the RPATH on a executable
# ---------------------------------------------------------------------------------
if(OPENNI_PLATFORM_APPLE)
        set_target_properties(${TINYXML_LIB_NAME} PROPERTIES 
                BUILD_WITH_INSTALL_RPATH ON
                INSTALL_NAME_DIR ${INSTALL_NAME_VALUE}
        )
endif()


install(TARGETS ${TINYXML_LIB_NAME}
       LIBRARY DESTINATION ${LIB_INSTALL_PATH}
       RUNTIME DESTINATION .
)
