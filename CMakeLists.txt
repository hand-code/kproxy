cmake_minimum_required(VERSION 2.8)
project(kproxy)

set(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -DPROJECT_VERSION=${PROJECT_VERSION} -Wall")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmakes)

set(SOURCE_FILES main.c common.h kp_server.c kp_utils.c)

INCLUDE (CheckFunctionExists)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckTypeSize)

# local system libraries
find_library (LIB_PTHREAD pthread)
find_library (LIB_DL dl)

set (SYS_LIBS
        ${LIB_PTHREAD}
        ${LIB_DL}
        )

if (APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__APPLE__")
endif(APPLE)

if (NOT APPLE)
    find_library (LIB_RT rt)
    set (SYS_LIBS ${SYS_LIBS} ${LIB_RT})
endif(NOT APPLE)

find_package(LibEvent)
find_package(libevhtp)
set(KPROXY_TARGET_LIBRARY
        ${LIBEVENT_LIBRARY}
        ${LIBEVENT_CORE_LIBRARY}
        ${LIBEVENT_PTHREADS_LIBRARY}
        ${LIBEVENT_EXTRA_LIBRARY}
        ${LIBEVENT_OPENSSL_LIBRARY}
        ${LIBEVHTP_LIBRARY}
        )

include_directories(${LIBEVENT_INCLUDE_DIR}
        ${LIBEVHTP_INCLUDE_DIR})

add_executable(kproxy ${SOURCE_FILES})
target_link_libraries(kproxy ${KPROXY_TARGET_LIBRARY})