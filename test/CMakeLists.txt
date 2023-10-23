include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
if (MSVC)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

add_executable(sbd_test
        serialization.cpp
        pages.cpp
        perf2.cpp)

target_link_libraries(
  sbd_test
  libsbd
  GTest::gtest_main
  gmock_main
)

target_include_directories(sbd_test PUBLIC ${SOURCE_DIR} ${TEST_DIR})

include(GoogleTest)
gtest_discover_tests(sbd_test)
