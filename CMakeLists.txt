cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED on)

project(ticketing VERSION 0.0.3)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(SQLITE REQUIRED sqlite3)
pkg_check_modules(JSONC REQUIRED json-c)
pkg_check_modules(LIBMUSTACH_JSONC REQUIRED libmustach-json-c)

find_package(SQLite3)

include_directories(
    ${GLIB_INCLUDE_DIRS}
    ${JSONC_INCLUDE_DIRS}
    ${LIBMUSTACH_JSONC_INCLUDE_DIRS}
)
link_directories(
    ${GLIB_LIBRARY_DIRS}
    ${SQLite3_LIBRARIES}
    ${JSONC_LIBRARY_DIRS}
    ${LIBMUSTACH_JSONC_LIBRARY_DIRS}
)

file(GLOB_RECURSE SRC_FILES src/*.c)
file(GLOB_RECURSE LIB_FILES lib/*.c)
add_executable(
    ticketing
    ${SRC_FILES}
    ${LIB_FILES}
)
target_compile_options(
    ticketing PUBLIC
    -W -Wall -Wextra -g
    -DMG_HTTP_DIRLIST_TIME_FMT="%Y/%m/%d %H:%M:%S"
    -DMG_ENABLE_LINES=1 -DMG_ENABLE_IPV6=1 -DMG_ENABLE_SSI=1
)
target_include_directories(ticketing PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data/ticket
    ${CMAKE_CURRENT_SOURCE_DIR}/src/site
    ${CMAKE_CURRENT_SOURCE_DIR}/src/site/default
)
target_link_libraries(
    ticketing
    ${SQLite3_LIBRARIES}
    ${GLIB_LINK_LIBRARIES}
    ${JSONC_LINK_LIBRARIES}
    ${LIBMUSTACH_JSONC_LINK_LIBRARIES}
)