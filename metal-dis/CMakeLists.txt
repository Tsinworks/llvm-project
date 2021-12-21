
find_package(LLVM REQUIRED HINTS "${LLVM_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(SRCS
MetalLib.h
MetalLib.cpp
AppleIRDump.cpp
Main.cpp
)

source_group(Metal FILES ${SRCS})

add_executable(metal-dis ${SRCS})

target_include_directories(metal-dis
  PUBLIC  ${LLVM_INCLUDE_DIRS})

target_compile_features(metal-dis PRIVATE cxx_std_14)

llvm_map_components_to_libnames(LLVM_LIBRARIES core)

target_link_libraries(metal-dis ${LLVM_LIBRARIES} LLVMBitReader)