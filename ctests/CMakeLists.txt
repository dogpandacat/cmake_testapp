cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
add_executable(ctest_exe_addvalues_zero addvalues_zero.cpp)
add_test(test1 ctest_exe_addvalues_zero)        # 对这个目标进行测试