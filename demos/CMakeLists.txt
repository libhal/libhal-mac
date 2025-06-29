# Copyright 2024 - 2025 Khalil Estell and the libhal contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.20)

project(mac_demos VERSION 0.0.1 LANGUAGES CXX)

# Generate compile commands for anyone using our libraries.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(libhal-mac REQUIRED CONFIG)

set(DEMOS serial)
foreach(DEMO ${DEMOS})
    message(STATUS "Generating Demo for \"${PROJECT_NAME}_${DEMO}")
    add_executable(${PROJECT_NAME}_${DEMO} main.cpp applications/${DEMO}.cpp)
    target_include_directories(${PROJECT_NAME}_${DEMO} PUBLIC .)
    target_compile_features(${PROJECT_NAME}_${DEMO} PRIVATE cxx_std_23)
    target_link_libraries(${PROJECT_NAME}_${DEMO} PRIVATE libhal::mac)
endforeach()
