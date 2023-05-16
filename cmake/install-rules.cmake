if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/multiarray"
      CACHE PATH ""
  )
  set(
      CMAKE_INSTALL_INCLUDEDIR_VISIT_STRUCT "include/visit_struct"
      CACHE PATH ""
  )
endif()

# Project is configured with no languages, so tell GNUInstallDirs the lib dir
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package multiarray)

install(
    DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT multiarray_Development
)

install(
    DIRECTORY submodules/visit_struct/include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR_VISIT_STRUCT}"
    COMPONENT multiarray_Development
)

install(
    TARGETS multiarray_multiarray
    EXPORT multiarrayTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" "${CMAKE_INSTALL_INCLUDEDIR_VISIT_STRUCT}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Allow package maintainers to freely override the path for the configs
set(
    multiarray_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(multiarray_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${multiarray_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT multiarray_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${multiarray_INSTALL_CMAKEDIR}"
    COMPONENT multiarray_Development
)

install(
    EXPORT multiarrayTargets
    NAMESPACE multiarray::
    DESTINATION "${multiarray_INSTALL_CMAKEDIR}"
    COMPONENT multiarray_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
