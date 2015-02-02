# Copyright (c) 2014 Ableton AG, Berlin
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

find_library(Rhea_LIBRARY rhea)

add_library(Rhea::Rhea UNKNOWN IMPORTED)
if (Rhea_LIBRARY)
  set_property(TARGET Rhea::Rhea PROPERTY IMPORTED_LOCATION "${Rhea_LIBRARY}")
  get_filename_component(_dir ${Rhea_LIBRARY} PATH)
  get_filename_component(_dir ${_dir} PATH)
  set_property(TARGET Rhea::Rhea PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${_dir}/include")
else()
  include(ExternalProject)
  set(rhea_CMake_args
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/rhea-prefix
    -DCMAKE_POLICY_DEFAULT_CMP0042=NEW
    )
  if(UNIX)
    set(rhea_CMake_args ${rhea_CMake_args} -DCMAKE_CXX_FLAGS=-fPIC)
  endif()
  ExternalProject_Add(RheaBuild
    SOURCE_DIR "${AqtCassowary_SOURCE_DIR}/third-party/rhea"
    CMAKE_ARGS ${rhea_CMake_args}
    )

  if(WIN32)
    set(rhea_lib_location "${CMAKE_CURRENT_BINARY_DIR}/rhea-prefix/lib/rhea-s.lib")
  else()
    set(rhea_lib_location "${CMAKE_CURRENT_BINARY_DIR}/rhea-prefix/lib/librhea-s.a")
  endif()
  set_property(TARGET Rhea::Rhea PROPERTY IMPORTED_LOCATION "${rhea_lib_location}")
  add_dependencies(Rhea::Rhea RheaBuild)
  set(Rhea_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/rhea-prefix/include")
endif()
if(WIN32)
  # The rhea buildsystem unconditionally adds /MT flags (for static builds)
  # That means we have to do the same.
  set_property(TARGET Rhea::Rhea PROPERTY INTERFACE_COMPILE_OPTIONS
    "$<$<CONFIG:Debug>:/MTd>"
    "$<$<NOT:$<CONFIG:Debug>>:/MT>"
  )
endif()

add_library(AqtCassowaryPlugin MODULE
  aqt/cassowary/CassowaryPlugin.cpp
  aqt/cassowary/CassowaryPlugin.hpp
  aqt/cassowary/Constraint.cpp
  aqt/cassowary/Constraint.hpp
  aqt/cassowary/ConstraintBase.cpp
  aqt/cassowary/ConstraintBase.hpp
  aqt/cassowary/Context.cpp
  aqt/cassowary/Context.hpp
  aqt/cassowary/Contextual.cpp
  aqt/cassowary/Contextual.hpp
  aqt/cassowary/Edit.cpp
  aqt/cassowary/Edit.hpp
  aqt/cassowary/Solver.cpp
  aqt/cassowary/Solver.hpp
  aqt/cassowary/Stay.cpp
  aqt/cassowary/Stay.hpp
  aqt/cassowary/Strength.cpp
  aqt/cassowary/Strength.hpp
  aqt/cassowary/Variable.cpp
  aqt/cassowary/Variable.hpp
  aqt/cassowary/Targeted.cpp
  aqt/cassowary/Targeted.hpp
  )
target_link_libraries(AqtCassowaryPlugin Rhea::Rhea Qt5::Quick)
target_include_directories(AqtCassowaryPlugin PUBLIC
  "${PROJECT_SOURCE_DIR}/compat/src"
  "${Rhea_INCLUDE_DIR}"
  "${Boost_INCLUDE_DIRS}")
target_compile_options(AqtCassowaryPlugin
  PUBLIC ${cxx11_options} ${warning_options})

set(plugin_output "${PROJECT_BINARY_DIR}/output")
set_target_properties(AqtCassowaryPlugin
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${plugin_output})
set_target_properties(AqtCassowaryPlugin
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${plugin_output})
set_target_properties(AqtCassowaryPlugin
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${plugin_output})

# rewrite qmldir to the plugin in the output folder (PATH_TO_PLUGIN is used
# in qmldir.in)
set(PATH_TO_PLUGIN "${PROJECT_BINARY_DIR}/output")
configure_file(
  "${PROJECT_SOURCE_DIR}/qml/Aqt/Cassowary/qmldir.in"
  "${PROJECT_SOURCE_DIR}/qml/Aqt/Cassowary/qmldir"
  @ONLY)
unset(PATH_TO_PLUGIN)

# prepare another qmldir, which we can use for installation
set(PATH_TO_PLUGIN "")
configure_file(
  "${PROJECT_SOURCE_DIR}/qml/Aqt/Cassowary/qmldir.in"
  "${plugin_output}/qmldir"
  @ONLY)
unset(PATH_TO_PLUGIN)

set(PLUGIN_INSTALL_DIR "lib/qml")
install(TARGETS AqtCassowaryPlugin
  LIBRARY DESTINATION "${PLUGIN_INSTALL_DIR}/Aqt/Cassowary")
install(FILES "${plugin_output}/qmldir"
  DESTINATION "${PLUGIN_INSTALL_DIR}/Aqt/Cassowary")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/qml/Aqt/Cassowary"
  DESTINATION "${PLUGIN_INSTALL_DIR}"
  FILES_MATCHING REGEX ".*\\.(qml|js)$")

# add tests
add_test(AqtCassowaryTestCase
  qmltestrunner
  -import "${PROJECT_SOURCE_DIR}/qml"
  -import "${PROJECT_SOURCE_DIR}/compat/qml"
  -input "${PROJECT_SOURCE_DIR}/qml")
