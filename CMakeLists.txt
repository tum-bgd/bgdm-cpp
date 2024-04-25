cmake_minimum_required(VERSION 3.12)

project(
  "bgdm-cpp"
  VERSION 1.0.0
    DESCRIPTION
    "A header-only c++ library providing selected algorithms from and for BGDM."
    HOMEPAGE_URL  "https://github.com/tum-bgd/bgdm-cpp")

include(GNUInstallDirs)

add_library(bgdm-cpp INTERFACE)


# Adding the install interface generator expression makes sure that the include
# files are installed to the proper location (provided by GNUInstallDirs)
target_include_directories(
  bgdm-cpp
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(bgdm-cpp INTERFACE cxx_std_11)

# no tests yet
# enable_testing()
# add_subdirectory(test)

# locations are provided by GNUInstallDirs
install(TARGETS bgdm-cpp
        EXPORT bgdm-cpp_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("bgdm-cppConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)


#installation currently not wanted.
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/bgdm-cppConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/bgdm-cppConfig.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/bgdm-cpp/cmake)

install(EXPORT bgdm-cpp_Targets
        FILE bgdm-cppTargets.cmake
        NAMESPACE bgdm-cpp::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bgdm-cpp/cmake)

install(FILES "${PROJECT_BINARY_DIR}/bgdm-cppConfig.cmake"
              "${PROJECT_BINARY_DIR}/bgdm-cppConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bgdm-cpp/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/bgdm DESTINATION include)

