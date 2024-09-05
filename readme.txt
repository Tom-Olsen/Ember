How to compile (Windows 10):
Clone repository with submodules: git clone --recurse-submodules <repository-url>
Create build directory in root folder of repository.

Start CMake and use following configuration:
Where is the source code: <full path to repository, e.g. C:/Users/.../Documents/c++/MyEngine>
Where to build the binaries: <full path to build folder, e.g. C:/Users/.../Documents/c++/MyEngine/build>
Press the configure button.

You should now have a visual studio project in the build folder named MyEngine.sln.
Open the .sln with visual studio 2022 or later, right click the 'MyEngine' solution and selecht 'Set as Startup Project'.
You can now build and run the code with the play button or with 'ctrl + F5'.



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
