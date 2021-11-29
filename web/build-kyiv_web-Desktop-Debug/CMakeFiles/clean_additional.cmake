# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/kyiv_web_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/kyiv_web_autogen.dir/ParseCache.txt"
  "kyiv_web_autogen"
  )
endif()
