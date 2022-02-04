set(SRCS Main.cpp)

source_group(src FILES ${SRCS})

set(LLVM_LINK_COMPONENTS
  Option
  Support
  )

add_clang_tool(win32-rpc
  ${SRCS}
  )

clang_target_link_libraries(win32-rpc
  PRIVATE
  clangAST
  clangBasic
  clangFormat
  clangFrontend
  clangLex
  clangRewrite
  clangSerialization
  clangTooling
  clangToolingCore
  clangToolingRefactoring
  )