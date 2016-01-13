cmake_minimum_required(VERSION 3.3)
project(DeviantArt_D)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(dir D:/GameTmp/dev/)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})
set(CMAKE_BUILD_FILES_DIRECTORY ${dir})
set(CMAKE_BUILD_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR ${dir})
SET(EXECUTABLE_OUTPUT_PATH ${dir})
SET(LIBRARY_OUTPUT_PATH ${dir}lib)
SET(CMAKE_CACHEFILE_DIR ${dir})

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5Declarative)

set(CMAKE_CXX_FLAGS "-std=c++11")
# -fPIC ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}

qt5_wrap_cpp(GENERATED_SOURCES mtoken.h)

add_executable(DevsGet DevsGet/main.cpp mtoken.cpp DevsGet/main.h)
qt5_use_modules(DevsGet Core Network)
target_compile_features(DevsGet PRIVATE cxx_range_for)
target_link_libraries(DevsGet ${Qt5Core_QTMAIN_LIBRARIES})

add_executable(Download Download/main.cpp mtoken.cpp Download/main.h)
qt5_use_modules(Download Core Widgets Network)
target_compile_features(Download PRIVATE cxx_range_for)
target_link_libraries(Download ${Qt5Core_QTMAIN_LIBRARIES})

add_executable(FoldersGet FoldersGet/main.cpp mtoken.cpp FoldersGet/main.h)
qt5_use_modules(FoldersGet Core Network)
target_compile_features(FoldersGet PRIVATE cxx_range_for)
target_link_libraries(FoldersGet ${Qt5Core_QTMAIN_LIBRARIES})

add_executable(Sorter Sorter/main.cpp mtoken.cpp Sorter/main.h)
qt5_use_modules(Sorter Core Network)
target_compile_features(Sorter PRIVATE cxx_range_for)
target_link_libraries(Sorter ${Qt5Core_QTMAIN_LIBRARIES})

add_executable(Runner Runner/main.cpp)

