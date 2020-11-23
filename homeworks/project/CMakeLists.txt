cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(GAMES102_Project VERSION 0.0.2)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

set(Ubpa_USRefl_Build_AutoRefl TRUE CACHE BOOL "use auto refl" FORCE)

Ubpa_AddDep(Utopia 0.0.5)
Ubpa_AddDep(UHEMesh 0.5.3)

Ubpa_DownloadZip(
  http://home.ustc.edu.cn/~ustczt/GAMES102/assets_v2.zip
  assets_v2.zip
  SHA256 B398C07B0A4F6C97ED2E3788F5CCF502D2DE332CD0A6A9C1BB677FB9F0C01D43
)

Ubpa_AddSubDirsRec(src)
