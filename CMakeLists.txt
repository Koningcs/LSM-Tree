# CMake 最低版本要求
cmake_minimum_required(VERSION 3.14)

# 项目名称
project(LSMTree VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准: C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
# 这样就可以在代码中 #include <lsm/lsm_tree.h>
include_directories(${PROJECT_SOURCE_DIR}/include)

# ========================================
# 构建 LSM-Tree 库
# ========================================
# 收集所有源文件（递归查找 src 目录）
file(GLOB_RECURSE LSM_SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# 创建静态库
# 注意：这里排除了 main.cpp（如果有的话）
if(LSM_SOURCES)
    add_library(lsm_tree_lib STATIC ${LSM_SOURCES})
    target_include_directories(lsm_tree_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
endif()

# ========================================
# 配置 Google Test
# ========================================
# 启用测试
enable_testing()

# Windows 下的特殊设置
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# 使用本地的 Google Test
# 请将 googletest 下载到 third_party/googletest 目录
if(EXISTS "${PROJECT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt")
    message(STATUS "Using local Google Test from third_party/googletest")
    add_subdirectory(third_party/googletest)
else()
    # 如果本地没有，则从网络下载
    message(STATUS "Downloading Google Test from GitHub")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
endif()

# ========================================
# 构建测试可执行文件
# ========================================
# 收集所有测试文件
file(GLOB_RECURSE TEST_SOURCES
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
)

# 创建测试可执行文件
if(TEST_SOURCES)
    add_executable(lsm_tree_test ${TEST_SOURCES})

    # 链接 Google Test 库
    target_link_libraries(lsm_tree_test
        gtest_main  # gtest_main 包含 main 函数
    )

    # 链接我们自己的库

    if(LSM_SOURCES)
        target_link_libraries(lsm_tree_test lsm_tree_lib)
    endif()

    # 添加到 CTest
    include(GoogleTest)
    gtest_discover_tests(lsm_tree_test)
endif()

# ========================================
# 打印配置信息（可选）
# ========================================
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Source Files: ${LSM_SOURCES}")
message(STATUS "Test Files: ${TEST_SOURCES}")
