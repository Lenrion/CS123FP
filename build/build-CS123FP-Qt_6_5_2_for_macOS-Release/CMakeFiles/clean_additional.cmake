# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/CS123FP_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/CS123FP_autogen.dir/ParseCache.txt"
  "CMakeFiles/StaticGLEW_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/StaticGLEW_autogen.dir/ParseCache.txt"
  "CS123FP_autogen"
  "StaticGLEW_autogen"
  )
endif()
