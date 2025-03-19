Prerequisites:
To compile this code, you need to have the Vulkan SDK 1.3.290.0 or later installed.
All other dependencies (SDL, vma, SPIRV-Reflect, imgui, spdlog) are submodules and will be compiled from the source.
(Don't forget to clone recursively for the submodules: git clone --recurse-submodules <repository-url>)

The Engine folder contains a CMake project that builds the ember engine into a static library.
Any project then must include the EmberEngine as a target_link_libraries(...).
Examples of this setup can be found in Projects/<Project Name>/CMakeLists.txt.

How to compile (Windows 10):
Start CMake and use the following configuration:
Where is the source code: <full path to repository, e.g. C:/.../Ember/Projects/<Project Name>>
Where to build the binaries: <full path to build folder, e.g. C:/.../Ember/Projects/<Project Name>/build>
Press the buttons 'configure' and then 'generate'.

You should now have a vs project <Project Name>.sln in the build folder.
Open the .sln with Visual Studio 2022 or later, right-click the <Project Name> solution, and select 'Set as Startup Project'.
You can now build and run the code with the play button or 'ctrl + F5'.



Useful Information:
Device Info: https://vulkan.gpuinfo.org/
Vulkan documentation: https://vulkan.lunarg.com/doc/sdk
