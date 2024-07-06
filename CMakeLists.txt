cmake_minimum_required(VERSION 3.22.4 FATAL_ERROR)
cmake_policy(SET CMP0127 NEW)
cmake_policy(SET CMP0135 NEW)

if(NOT DEFINED PROJECT_NAME)
    set(XYZ_DYN_OPTIONAL_IS_NOT_SUBPROJECT ON)
endif()

set(XYZ_DYN_OPTIONAL_VERSION 0.0.1)

project(value_types LANGUAGES CXX VERSION ${XYZ_DYN_OPTIONAL_VERSION})

include(CTest)
include(FetchContent)
include(GNUInstallDirs)
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)

# Ensure code coverage is switched off for Mac OS until the code coverage library addresses the AppleClang issue
cmake_dependent_option(ENABLE_CODE_COVERAGE "Enable code coverage" ON "\"${CMAKE_CXX_COMPILER_ID}\" STREQUAL \"Clang\" OR \"${CMAKE_CXX_COMPILER_ID}\" STREQUAL \"GNU\"" OFF)
option(ENABLE_SANITIZERS "Enable Address Sanitizer and Undefined Behaviour Sanitizer if available" OFF)

# Include necessary submodules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Custom CMake Includes
include(xyz_add_library)
include(xyz_add_test)

xyz_add_library(
    NAME dyn_optional
    ALIAS xyz_dyn_optional::dyn_optional
)
target_sources(dyn_optional
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dyn_optional.h>
)

if (${XYZ_DYN_OPTIONAL_IS_NOT_SUBPROJECT})

    if(!MSVC)
        # There are issues with constexpr/consteval in MSVC 2019 and MSVC 2022.
        add_subdirectory(compile_checks)
    endif(!MSVC)

    if (${BUILD_TESTING})
        FetchContent_Declare(
          googletest
          URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)

        enable_testing()

        if (ENABLE_CODE_COVERAGE)
            FetchContent_Declare(
                codecoverage
                GIT_REPOSITORY https://github.com/RWTH-HPC/CMake-codecov.git
            )

            FetchContent_GetProperties(codecoverage)
            if(NOT codecoverage_POPULATED)
                FetchContent_Populate(codecoverage)
                list(APPEND CMAKE_MODULE_PATH ${codecoverage_SOURCE_DIR}/cmake)
            endif()

            set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
            find_package(codecov)
            list(APPEND LCOV_REMOVE_PATTERNS "'/usr/*'")
        endif()

        xyz_add_test(
            NAME dyn_optional_test
            LINK_LIBRARIES dyn_optional
            FILES dyn_optional_test.cc
            VERSION 17
        )

        if (ENABLE_CODE_COVERAGE)
            coverage_evaluate()
        endif()

    endif(${BUILD_TESTING})

endif()
