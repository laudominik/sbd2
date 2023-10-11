add_executable(${PROJECT_NAME} ${SOURCE_DIR}/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_DIR})

add_library(libsbd STATIC)
target_include_directories(libsbd PUBLIC ${SOURCE_DIR} ${SOURCE_DIR}/generic)
add_subdirectory(${SOURCE_DIR}/impl)

target_link_libraries(${PROJECT_NAME} PRIVATE libsbd)
