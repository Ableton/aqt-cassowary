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

add_library(AqtCassowaryPlugin MODULE
  ableton/cassowary/CassowaryPlugin.cpp
  ableton/cassowary/CassowaryPlugin.hpp
  ableton/cassowary/Constraint.cpp
  ableton/cassowary/Constraint.hpp
  ableton/cassowary/ConstraintBase.cpp
  ableton/cassowary/ConstraintBase.hpp
  ableton/cassowary/Context.cpp
  ableton/cassowary/Context.hpp
  ableton/cassowary/Contextual.cpp
  ableton/cassowary/Contextual.hpp
  ableton/cassowary/Edit.cpp
  ableton/cassowary/Edit.hpp
  ableton/cassowary/Solver.cpp
  ableton/cassowary/Solver.hpp
  ableton/cassowary/Stay.cpp
  ableton/cassowary/Stay.hpp
  ableton/cassowary/Strength.cpp
  ableton/cassowary/Strength.hpp
  ableton/cassowary/Variable.cpp
  ableton/cassowary/Variable.hpp
  ableton/cassowary/Targeted.cpp
  ableton/cassowary/Targeted.hpp
  )
target_link_libraries(AqtCassowaryPlugin rhea Qt5::Quick)
target_include_directories(AqtCassowaryPlugin PUBLIC
  "${PROJECT_SOURCE_DIR}/compat/src"
  "${PROJECT_SOURCE_DIR}/third-party/rhea"
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
  "${PROJECT_SOURCE_DIR}/qml/Ableton/Cassowary/qmldir.in"
  "${PROJECT_SOURCE_DIR}/qml/Ableton/Cassowary/qmldir"
  @ONLY)
unset(PATH_TO_PLUGIN)

# prepare another qmldir, which we can use for installation
set(PATH_TO_PLUGIN "")
configure_file(
  "${PROJECT_SOURCE_DIR}/qml/Ableton/Cassowary/qmldir.in"
  "${plugin_output}/qmldir"
  @ONLY)
unset(PATH_TO_PLUGIN)

set(PLUGIN_INSTALL_DIR "lib/qml")
install(TARGETS AqtCassowaryPlugin
  LIBRARY DESTINATION "${PLUGIN_INSTALL_DIR}/Ableton/Cassowary")
install(FILES "${plugin_output}/qmldir"
  DESTINATION "${PLUGIN_INSTALL_DIR}/Ableton/Cassowary")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/qml/Ableton/Cassowary"
  DESTINATION "${PLUGIN_INSTALL_DIR}"
  FILES_MATCHING REGEX ".*\\.(qml|js)$")

# add tests
add_test(AqtCassowaryTestCase
  qmltestrunner
  -import "${PROJECT_SOURCE_DIR}/qml"
  -import "${PROJECT_SOURCE_DIR}/compat/qml"
  -input "${PROJECT_SOURCE_DIR}/qml")
