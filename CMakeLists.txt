cmake_minimum_required(VERSION 2.8)


set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
mark_as_advanced(CMAKE_CONFIGURATION_TYPES)


project(opcodes_dumper)


if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  set(branch "64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^i.86$")
  set(branch "32")
else()
  message(FATAL_ERROR "Cannot determine system processor type!")
endif()

find_path(
  PHP_INCLUDE_PATH

  NAMES
  Zend/zend_vm_opcodes.h

  PATHS
  ${PROJECT_SOURCE_DIR}/externals/${branch}/include
)
if ( NOT PHP_INCLUDE_PATH )
  message(FATAL_ERROR "Couldn't find PHP include path")
endif ( )

find_library(
  PHP_LIBRARY

  NAMES
  libphp5.a

  PATHS
  ${PROJECT_SOURCE_DIR}/externals/${branch}/lib
)
if ( NOT PHP_LIBRARY )
  message(FATAL_ERROR "Couldn't find PHP embedded library")
endif ( )


#########################################################################
# Convert opcodes into C-file
file(READ "${PHP_INCLUDE_PATH}/Zend/zend_vm_opcodes.h" OPCODES)
string(REGEX MATCHALL "[ \t]*\#[ \t]*define[ \t]*([^ \t]+)[ \t]*([0-9]+)" OPDEF "${OPCODES}")
file(WRITE  "${CMAKE_BINARY_DIR}/opcodes.c" "#include \"${CMAKE_SOURCE_DIR}/opcodes.h\"\n")
file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "#include \"${PHP_INCLUDE_PATH}/Zend/zend_vm_opcodes.h\"\n")
file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "\n")
file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "\n")
file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "const char* get_opname(zend_uchar opcode) {\n")
file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "  switch( opcode ) {\n")

foreach(loop_var ${OPDEF})
  string(REGEX REPLACE "[ \t]*\#[ \t]*define[ \t]*([^ \t]+)[ \t]*([0-9]+)" "  case \\1: return \"\\1\"; break;\n" amatch "${loop_var}")
  file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "${amatch}")
endforeach(loop_var)

file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "  default: return \"UNKNOWN_OPCODE\";\n")
file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "  };\n")
file(APPEND "${CMAKE_BINARY_DIR}/opcodes.c" "}\n")
#########################################################################


include_directories(
  ${PHP_INCLUDE_PATH}
  ${PHP_INCLUDE_PATH}/Zend
  ${PHP_INCLUDE_PATH}/TSRM
  ${PHP_INCLUDE_PATH}/main
)


add_executable(
  ${PROJECT_NAME}

  opcodes.h
  opcodes.c
  opcodes_dumper.cpp
)


target_link_libraries(
  ${PROJECT_NAME}

  ${PHP_LIBRARY}
  dl
  resolv
)


if( CMAKE_BUILD_TYPE STREQUAL "DEBUG" )
  # Maybe add some other special debug commands here? Maybe add -ggdb switch?
else()
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} ${PROJECT_NAME})
endif()
