// Copyright 2024 - 2025 Khalil Estell and the libhal contributors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Application function must be implemented by one of the compilation units
// (.cpp) files.
extern void application();

int main()
{
  // Add system initialization code here such as changing system clock speed.
  // Add necessary code here or delete this and the comment above...

  // Set terminate routine...

  // Run application
  application();

  // Reset the device if it gets here
  return 0;
}
