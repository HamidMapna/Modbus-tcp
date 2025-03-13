#********************************************************************
#        _       _         _
#  _ __ | |_  _ | |  __ _ | |__   ___
# | '__|| __|(_)| | / _` || '_ \ / __|
# | |   | |_  _ | || (_| || |_) |\__ \
# |_|    \__|(_)|_| \__,_||_.__/ |___/
#
# www.rt-labs.com
# Copyright 2019 rt-labs AB, Sweden.
#
# This software is dual-licensed under GPLv3 and a commercial
# license. See the file LICENSE.md distributed with this software for
# full license information.
#*******************************************************************/

cmake_minimum_required (VERSION 3.14)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/tools")
project (MBUS VERSION 0.1.0)

include(AddOsal)
include(GenerateExportHeader)
include(CMakeDependentOption)

set(LIBXML2_INCLUDE_DIR /usr/include/libxml2/)
set(DATABASE_FILE /usr/local/share/xml/db.xml)
set(XML_DIR ${MBUS_SOURCE_DIR}/db/)

set(LOG_STATE_VALUES "ON;OFF")
set(LOG_LEVEL_VALUES "DEBUG;INFO;WARNING;ERROR")

set(LOG_LEVEL ERROR CACHE STRING "default log level")
set_property(CACHE LOG_LEVEL PROPERTY STRINGS ${LOG_LEVEL_VALUES})

set(MB_RTU_LOG ON CACHE STRING "rtu log")
set_property(CACHE MB_RTU_LOG PROPERTY STRINGS ${LOG_STATE_VALUES})

set(MB_TCP_LOG ON CACHE STRING "tcp log")
set_property(CACHE MB_TCP_LOG PROPERTY STRINGS ${LOG_STATE_VALUES})

set(MBUS_TIMEOUT "100"
  CACHE STRING "timeout in ms for something")

# Pass database path to the code as a preprocessor definition
add_definitions(-DDATABASE_FILE="${DATABASE_FILE}")
# Generate version numbers
configure_file (
  version.h.in
  ${MBUS_BINARY_DIR}/src/version.h
  )

# Generate config options
configure_file (
  options.h.in
  ${MBUS_BINARY_DIR}/src/options.h
  )

# Add platform-dependent targets early, so they can be configured by
# platform
add_library(mbus "")
add_executable(mb_master ${CMAKE_SOURCE_DIR}/src/ports/linux/tcp_rtu_master.c)

# Platform configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CMAKE_SYSTEM_NAME}.cmake)

generate_export_header(mbus
  BASE_NAME mb
  EXPORT_FILE_NAME ${MBUS_BINARY_DIR}/include/mb_export.h
  )

set_target_properties (mbus mb_master 
  PROPERTIES
  C_STANDARD 99
  )

target_compile_features(mbus
  PUBLIC
  c_std_99
  PRIVATE
  cxx_std_11)

include_directories(${MBUS_SOURCE_DIR}/db/)

target_include_directories(mbus
  PUBLIC
  $<BUILD_INTERFACE:${MBUS_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${MBUS_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE
  ${LIBXML2_INCLUDE_DIR}/
  ${MBUS_BINARY_DIR}/src
  src
  )

target_link_libraries(mbus PUBLIC osal xml2)
target_link_libraries(mb_master PUBLIC mbus)

install (
  TARGETS mbus
  EXPORT MbusConfig
  DESTINATION lib
  )

install(
  EXPORT MbusConfig
  DESTINATION cmake
  )

install (FILES
  include/mb_master.h
  include/mb_slave.h
  include/mb_transport.h
  include/mb_rtu.h
  include/mb_tcp.h
  include/mb_error.h
  ${MBUS_BINARY_DIR}/include/mb_export.h
  DESTINATION include
  )
  #install db.xml file into target
install(FILES ${XML_DIR}/db.xml DESTINATION /usr/local/share/xml/)
 
add_subdirectory(./libs libs)
add_subdirectory (src)

