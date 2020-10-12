cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(GAMES102_HW VERSION 0.0.1)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

set(Ubpa_USRefl_Build_AutoRefl TRUE CACHE BOOL "use auto refl" FORCE)

Ubpa_AddDep(Utopia 0.0.3)

Ubpa_DownloadZip(
  http://home.ustc.edu.cn/~ustczt/GAMES102/assets_v1.zip
  assets_v1.zip
  SHA256 B63CA7F40C818C9C97C17A99C064F1CC24D087280BFEDA43D371B55E12650507
)

Ubpa_AddSubDirsRec(src)
