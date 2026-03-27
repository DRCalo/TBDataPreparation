cmake_minimum_required(VERSION 3.16)

project(TBDataPreparation LANGUAGES CXX)

include(GNUInstallDirs)

# preparing for using ROOT

find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)

# ------------------------------------------------------------------
# Expected layout:
#   <workspace>/TBDataPreparation
#   <workspace>/build
#   <workspace>/run
#
# If the user does:
#   mkdir ../build && cd ../build
#   cmake ../TBDataPreparation
#
# then install goes by default to ../run
# ------------------------------------------------------------------
get_filename_component(WORKSPACE_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${WORKSPACE_DIR}/build" CACHE PATH "Install prefix" FORCE)
endif()

# ------------------------------------------------------------------
# Only 2025_SPS matters
# ------------------------------------------------------------------
set(TBVER "2025_SPS")
set(SIPM_DIR "${CMAKE_SOURCE_DIR}/${TBVER}/SIPM")

# Preparing to compile PhysicsHelpers

set(PHYSICS_SRC
    ${CMAKE_SOURCE_DIR}/2025_SPS/src/PhysicsHelper.cxx
)

set(PHYSICS_HEADERS
    ${CMAKE_SOURCE_DIR}/2025_SPS/include/PhysicsHelper.h
)

add_library(PhysicsHelper SHARED
  ${PHYSICS_SRC}
)

ROOT_GENERATE_DICTIONARY(PhysicsHelperDict
    ${PHYSICS_HEADERS}
    LINKDEF ${CMAKE_SOURCE_DIR}/2025_SPS/include/LinkDef.h
    MODULE PhysicsHelper
    OPTIONS -I${CMAKE_SOURCE_DIR}/2025_SPS/PMT -I${CMAKE_SOURCE_DIR}/2025_SPS/include
)



target_include_directories(PhysicsHelper
    PUBLIC
        ${CMAKE_SOURCE_DIR}/2025_SPS/include
        ${CMAKE_SOURCE_DIR}/2025_SPS/PMT
)

target_link_libraries(PhysicsHelper
    PUBLIC ROOT::Core ROOT::RIO ROOT::Tree
)

set_target_properties(PhysicsHelper PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib
)

# Reuse the existing SIPM build as-is
add_subdirectory("${SIPM_DIR}")

# ------------------------------------------------------------------
# Generate setup.sh in the build tree, then install it into run/
# ------------------------------------------------------------------
set(SETUP_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/setup.sh")

# Define values you want to inject
set(IDEADIR      "${CMAKE_INSTALL_PREFIX}/..")
set(RUN_DIR      "${CMAKE_INSTALL_PREFIX}/../run")
set(SamplePath   "${IDEADIR}")
set(IDEARepo     "${IDEADIR}/TBDataPreparation")

set(LD_LIBRARY_PATH_SETUP "${CMAKE_INSTALL_PREFIX}/lib")
set(PATH_SETUP      "${CMAKE_INSTALL_PREFIX}/bin")
set(PYTHONPATH_SETUP "${CMAKE_INSTALL_PREFIX}/bin")
set(BATCHPATH       "${PATH_SETUP}")

# Configure file
configure_file(
  "${CMAKE_SOURCE_DIR}/setup.sh.in"
  "${CMAKE_CURRENT_BINARY_DIR}/setup.sh"
  @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libPhysicsHelper_rdict.pcm
    DESTINATION lib
    OPTIONAL
)

install(PROGRAMS
  ${CMAKE_SOURCE_DIR}/DreamDaqMon/AdcMap25.py
  ${CMAKE_SOURCE_DIR}/DreamDaqMon/DREvent.py
  ${CMAKE_SOURCE_DIR}/DreamDaqMon/decode_utils.py
  ${CMAKE_SOURCE_DIR}/2025_SPS/scripts/DoPhysicsConverter.py
  ${CMAKE_SOURCE_DIR}/2025_SPS/scripts/DR_makeRootFiles.py
  ${CMAKE_SOURCE_DIR}/2025_SPS/scripts/DRrootify.py
  ${CMAKE_SOURCE_DIR}/2025_SPS/scripts/DR_createMergeFromSiPMOnly.py
  ${CMAKE_SOURCE_DIR}/2025_SPS/scripts/bzipPMTfiles.py
  ${CMAKE_SOURCE_DIR}/2025_SPS/SIPM/scripts/SiPMConvert.py
  DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/2025_SPS/MapAndCalibration/
    DESTINATION calibration
)

install(CODE "
  file(MAKE_DIRECTORY \"${RUN_DIR}\")
")

set(TB_SCRIPTS
    DoPhysicsConverter.py
    DR_makeRootFiles.py
    DRrootify.py
    DR_createMergeFromSiPMOnly.py
    bzipPMTfiles.py
)

install(CODE "
  file(MAKE_DIRECTORY \"${RUN_DIR}\")
")
foreach(script IN LISTS TB_SCRIPTS)
  install(CODE "
    execute_process(
      COMMAND \"\${CMAKE_COMMAND}\" -E create_symlink
              \"${CMAKE_INSTALL_PREFIX}/bin/${script}\"
              \"${RUN_DIR}/${script}\"
    )
  ")
endforeach()
