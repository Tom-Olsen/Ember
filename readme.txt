Prerequisites:
To compile this code, you need to have the Vulkan SDK 1.3.290.0 or later installed.
All other dependencies (SDL, glm, spdlog) are submodules and will be compiled from the source.



How to compile (Windows 10):
Clone repository with submodules: git clone --recurse-submodules <repository-url>
Create a build directory in the root folder of the repository.

Start CMake and use the following configuration:
Where is the source code: <full path to repository, e.g. C:/Users/.../Documents/c++/MyEngine>
Where to build the binaries: <full path to build folder, e.g. C:/Users/.../Documents/c++/MyEngine/build>
Press the configure button.

You should now have a vs project named MyEngine.sln in the build folder.
Open the .sln with Visual Studio 2022 or later, right-click the 'MyEngine' solution, and select 'Set as Startup Project'.
You can now build and run the code with the play button or 'ctrl + F5'.



Useful Information:
Device Info: https://vulkan.gpuinfo.org/
Vulkan documentation: https://vulkan.lunarg.com/doc/sdk

Tutorials:
Official Tutorial: (Drawing a triangle/Swap chain recreation)
https://docs.vulkan.org/tutorial/latest/00_Introduction.html
German Tutorial: (Video 19)
https://www.youtube.com/watch?v=xtR3yWY-IBs&list=PLStQc0GqppuXgs6do23v_HKRrR32gJMm3&index=38
Englisch Tutorial: (not startet yet)
https://www.youtube.com/watch?v=keH_cPl5SSY&list=PLn3eTxaOtL2Nr89hYzKPib7tvce-ZO4yB&index=11
