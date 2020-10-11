cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(GAMES102_HW VERSION 0.0.1)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

set(Ubpa_USRefl_Build_AutoRefl TRUE CACHE BOOL "use auto refl" FORCE)

Ubpa_AddDep(Utopia 0.0.3)

Ubpa_DownloadZip(
  https://ubpa.github.io/GAMES102_Data/homework/project/assets/assets_v0.zip
  assets_v0.zip
  SHA256 186BF593547D88EC0ADB45A542C916DA221E2E34301CD77BCBC9FF4D35A8E2F7
)

Ubpa_AddSubDirsRec(src)
