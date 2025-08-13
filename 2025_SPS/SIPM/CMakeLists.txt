# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(SiPMConverter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find ROOT (modern imported targets)
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)
# include(${ROOT_USE_FILE}) # uncomment if you’re on an older ROOT needing it

# Public headers you want in the dictionary (adjust as needed)
set(PUBLIC_HEADERS
    ${PROJECT_SOURCE_DIR}/include/Decoder.h
    ${PROJECT_SOURCE_DIR}/include/Channel.h
    ${PROJECT_SOURCE_DIR}/include/Board.h
    ${PROJECT_SOURCE_DIR}/include/FileHeader.h
    ${PROJECT_SOURCE_DIR}/include/EventFragment.h
    ${PROJECT_SOURCE_DIR}/include/hardcoded.h
)

# Sources
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cxx)

# Generate ROOT dictionary for PyROOT/Cling autoloading
# Requires a LinkDef.h (template below)
set(DICT_NAME ${PROJECT_NAME}Dict)
ROOT_GENERATE_DICTIONARY(${DICT_NAME} ${PUBLIC_HEADERS}
    LINKDEF ${PROJECT_SOURCE_DIR}/include/LinkDef.h
    OPTIONS -I${PROJECT_SOURCE_DIR}/include
)

# Build shared library with the dictionary
add_library(${PROJECT_NAME} SHARED ${SRC} ${DICT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC ROOT::Core ROOT::RIO ROOT::Tree
)

# Keep outputs together so PyROOT can load them easily
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)

# (Optional) install rules
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES ${PUBLIC_HEADERS} DESTINATION include)

# (Optional) install PCM/rootmap if your ROOT version emits them separately
# They’re created next to the library/dict at build time.
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}_rdict.pcm
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rootmap
    DESTINATION lib
    OPTIONAL
)
