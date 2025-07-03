#!/usr/bin/python
#
# Copyright 2024 - 2025 Khalil Estell and the libhal contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from conan import ConanFile


required_conan_version = ">=2.18.0"


class libhal_mac_conan(ConanFile):
    name = "libhal-mac"
    license = "Apache-2.0"
    homepage = "https://libhal.github.io/libhal-mac"
    description = ("A collection of drivers and libraries for the Mac OS X "
                   "operating system")
    topics = ("mac", "osx", "darwin")
    settings = "compiler", "build_type", "os", "arch"

    python_requires = "libhal-bootstrap/[>=4.4.0 <5]"
    python_requires_extend = "libhal-bootstrap.library"

    def requirements(self):
        # Replace with appropriate processor library
        self.requires("libhal/[^4.12.0]", transitive_headers=True)
        self.requires("libhal-util/[^5.2.0]", transitive_headers=True)

    def package_info(self):
        self.cpp_info.set_property("cmake_target_name", "libhal::mac")
        self.cpp_info.libs = ["libhal-mac"]

    def package_id(self):
        if self.info.options.get_safe("platform"):
            del self.info.options.platform
        self.buildenv_info.define("LIBHAL_PLATFORM", "mac")
        self.buildenv_info.define("LIBHAL_PLATFORM_LIBRARY", "mac")
