
# project
cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0091 NEW)
project(m_log LANGUAGES CXX C)

ADD_DEFINITIONS(-DBUILD_SHARED_M_LOG)

# target
add_library(m_log SHARED "")
set_target_properties(m_log PROPERTIES OUTPUT_NAME "m_log")
# set_target_properties(m_log PROPERTIES 
#     POSITION_INDEPENDENT_CODE ON
#     WINDOWS_EXPORT_ALL_SYMBOLS ON
# )

target_include_directories(m_log PRIVATE ${CMAKE_SOURCE_DIR}/m_log)

find_package(fmt CONFIG REQUIRED)


target_link_libraries(m_log PUBLIC fmt::fmt)


target_sources(m_log PRIVATE
    log.cpp
    constant.h
    log.h
)

