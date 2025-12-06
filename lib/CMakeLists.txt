enable_language(C CXX)

add_library(hmr2300)
set(LIB_VERSION 0.1.0)
set_property(TARGET hmr2300 PROPERTY VERSION ${LIB_VERSION})
set_property(TARGET hmr2300 PROPERTY SOVERSION 0)
set_property(TARGET hmr2300 PROPERTY INTERFACE_hmr2300_MAJOR_VERSION 0)
set_property(TARGET hmr2300 APPEND PROPERTY COMPATIBLE_INTERFACE_STRING hmr2300_MAJOR_VERSION)

configure_file(hmr2300_config.h.in hmr2300/config.h @ONLY)

target_include_directories(hmr2300 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_include_directories(hmr2300 PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_sources(hmr2300 PRIVATE
    src/hmr2300.cpp
)

install(TARGETS hmr2300
    EXPORT hmr2300-config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/hmr2300/hmr2300ConfigVersion.cmake"
    VERSION ${LIB_VERSION}
    COMPATIBILITY SameMajorVersion
)

set(ConfigPackageLocation lib/hmr2300)

install(
    EXPORT hmr2300-config
    NAMESPACE hmr2300::
    DESTINATION
        ${ConfigPackageLocation}
    COMPONENT
        Devel)

install(DIRECTORY include/
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/
    COMPONENT
        Devel)
