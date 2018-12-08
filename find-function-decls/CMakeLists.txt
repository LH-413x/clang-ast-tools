set(LLVM_LINK_COMPONENTS support)

add_clang_executable(find-function-decls 
	FindFunctionDecls.cpp
	)

target_link_libraries(find-function-decls 
	PRIVATE
	clangTooling
	clangBasic
	clangASTMatchers
	)
