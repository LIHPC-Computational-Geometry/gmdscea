#==============================================================================
# GMDSCEA CORE MODULE
#==============================================================================
project(gmdscea CXX)

#add_definitions(-DDLLEXPORT)

#==============================================================================
# CODE STRUCTURE
#==============================================================================

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc")

set(GMDSCEA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(GMDSCEA_HEADER_FILES
        inc/GMDSCEA/GMDSCEAWriter.h
        inc/GMDSCEA/LimaReader.h
        inc/GMDSCEA/LimaReader_def.h
        inc/GMDSCEA/LimaWriter.h
        inc/GMDSCEA/LimaWriter_def.h
        inc/GMDSCEA/LimaWriterAPI.h
        )


set(GMDSCEA_SRC_FILES
        src/GMDSCEAWriter.cpp
        src/IGMeshWriterImplCEA.cpp
        src/LimaWriterAPI.cpp
        )


#==============================================================================
# DEPENDENCIES
#==============================================================================
find_package(gmds REQUIRED)

include_directories(${GMDS_INCLUDE_DIR})

find_package(Lima REQUIRED)

#==============================================================================
# TEST MODULE
#==============================================================================

#set(GMDS_TEST ON CACHE BOOL "GMDS Kernel test module")
#if (GMDS_TEST)
#  add_subdirectory(test)
#endif ()

#==============================================================================
# LIBRARY BUILDING
#==============================================================================

add_library(gmdscea ${GMDSCEA_HEADER_FILES} ${GMDSCEA_SRC_FILES})

#set_target_properties(gmds PROPERTIES PUBLIC_HEADER "${GMDS_HEADER_FILES}")


#target_compile_features(gmds PUBLIC cxx_std_11)


target_link_libraries(gmdscea PRIVATE gmds)
target_link_libraries(gmdscea PRIVATE Lima::Lima)

#add_custom_command(
#   TARGET gmds
#   POST_BUILD
#   COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory  $<TARGET_FILE:gmds> ${CMAKE_BINARY_DIR}/gen/
#   COMMENT "Copy gmds lib"
#   VERBATIM
#   )

#target_include_directories(gmds INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>/generated>)
target_include_directories(gmdscea PUBLIC "$<INSTALL_INTERFACE:include>")

install(TARGETS gmdscea
        EXPORT gmdscea DESTINATION
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )

install(FILES
        ${GMDSCEA_HEADER_FILES} DESTINATION include/GMDSCEA)


set(INCLUDE_INSTALL_DIR include/ )
set(LIB_INSTALL_DIR lib/ )
set(SYSCONFIG_INSTALL_DIR etc/gmds/ )

include(CMakePackageConfigHelpers)
configure_package_config_file(
        gmdsceaConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/gmdsceaConfig.cmake
        INSTALL_DESTINATION ${LIB_INSTALL_DIR}/gmdscea/cmake
        PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/gmdsceaConfigVersion.cmake
        VERSION ${GMDSCEA_SUITE_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/gmdsceaConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/gmdsceaConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/gmdscea/cmake
        )


install(EXPORT gmdscea DESTINATION lib/gmdscea)
