cmake_minimum_required (VERSION 3.9)
include(GNUInstallDirs)

project(vlwcc VERSION 1.0.0 DESCRIPTION "UCODDCC C library")

set(CMAKE_BUILD_TYPE Release)

set(SOURCES
        vlwcc.c
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES vlwcc.h
        DESTINATION /usr/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER vlwcc.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

find_package(Threads REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

target_compile_definitions (${PROJECT_NAME} PRIVATE -DDEBUG)
