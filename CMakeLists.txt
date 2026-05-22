cmake_minimum_required(VERSION 3.12...3.31)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
  cmake_policy(SET CMP0115 NEW)
endif()

if(NOT DEFINED SHOTCUT_VERSION)
  string(TIMESTAMP SHOTCUT_VERSION "%y.%m.%d")
  # Remove leading zero from month and day.
  string(REGEX REPLACE "\\.0([0-9])" ".\\1" SHOTCUT_VERSION ${SHOTCUT_VERSION})
endif()

project(bossa
  VERSION ${SHOTCUT_VERSION}
  DESCRIPTION "Bossa Video Editor - AI Powered & SODA Styled"
  HOMEPAGE_URL "https://bossa.io"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(FeatureSummary)

find_package(Qt6 REQUIRED COMPONENTS Widgets Quick QuickWidgets Xml Sql Network Multimedia OpenGL OpenGLWidgets Charts WebSockets QuickControls2)
find_package(PkgConfig REQUIRED)
pkg_check_modules(mlt++ REQUIRED IMPORTED_TARGET mlt++-7)
pkg_check_modules(FFTW REQUIRED IMPORTED_TARGET fftw3)

add_subdirectory(CuteLogger)
add_subdirectory(src)
add_subdirectory(translations)

# Formatting
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
  file(GLOB_RECURSE FORMAT_FILES
    "src/*.cpp" "src/*.h"
    "CuteLogger/src/*.cpp" "CuteLogger/include/*.h"
  )
  if(FORMAT_FILES)
    add_custom_target(format
      COMMAND ${CLANG_FORMAT} -style=file -i ${FORMAT_FILES}
    )
    add_custom_target(format-check
      COMMAND ${CLANG_FORMAT} --Werror -style=file -i ${FORMAT_FILES}
    )
  else()
    set(CLANG_FORMAT OFF)
  endif()
endif()

file(GLOB_RECURSE QML_SRC "src/qml/*.qml")
add_custom_target(qmlformat COMMAND
  ${CMAKE_PREFIX_PATH}/bin/qmlformat -i ${QML_SRC}
)
