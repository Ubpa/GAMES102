set(components
  DenoiseData
)

set(refls "")

foreach(cmpt ${components})
  set(dst "${CMAKE_CURRENT_SOURCE_DIR}/Components/details/${cmpt}_AutoRefl.inl")
  Ubpa_USReflAutoRefl(
    "${CMAKE_CURRENT_SOURCE_DIR}/Components/${cmpt}.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Components/details/${cmpt}_AutoRefl.inl"
  )
  list(APPEND refls ${dst})
endforeach()

Ubpa_AddTarget(
  TEST
  SOURCE
    ${refls}
  MODE EXE
  LIB
    Ubpa::Utopia_App_Editor
    Ubpa::UHEMesh_core
)
