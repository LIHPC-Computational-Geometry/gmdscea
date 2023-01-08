cmake_minimum_required(VERSION 3.14.0)

project(GMDSCEA_SUITE)

#==============================================================================
#Versionnage du projet

set(GMDSCEA_SUITE_VERSION_MAJOR 2)
set(GMDSCEA_SUITE_VERSION_MINOR 0)
set(GMDSCEA_SUITE_VERSION_PATCH 0)

set(GMDSCEA_SUITE_VERSION
        ${GMDSCEA_SUITE_VERSION_MAJOR}.${GMDSCEA_SUITE_VERSION_MINOR}.${GMDSCEA_SUITE_VERSION_PATCH})


option(BUILD_SHARED_LIBS    ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GMDSCEA)

