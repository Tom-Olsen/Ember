Prerequisites:
To compile this code, you need to have the Vulkan SDK 1.3.290.0 or later and cmake version 3.31 or later installed.
All other dependencies (SDL, vma, SPIRV-Reflect, imgui, spdlog, googletest) are submodules and will be compiled from source.
(Don't forget to clone recursively for the submodules: git clone --recurse-submodules <repository-url>)



Architecture: (knowing this is not important if you just want to create a project)
The folder 'engine/core' contains the file 'coreBuild.cmake' which contains a function which allows for different kind of builds of the core (e.g. game/editor/headless).
The different modes of the core are build in the folders 'engine/core/*Mode', which each contain a 'CMakeLists.txt' project.
The different modes are then linked into the corresponding applications in 'engine/application/*' also via cmake.



Project Setup:
To create a new project simply link the desired base application from 'engine/application/*' in your CMakeLists.txt via:
    add_subdirectory("../engine/applications/*" Application) # choose: * = editor/game/headless
    target_link_libraries(${PROJECT_NAME} PUBLIC Application)
Examples of this setup can be found in 'defaultProject/CMakeLists.txt'.



How to compile (Windows 10):
Start CMake and use the following configuration:
Where is the source code:    <parent folder of CMakeLists.txt, e.g. C:/.../Ember/defaultProject>
Where to build the binaries: <path to build folder,            e.g. C:/.../Ember/defaultProject/build>
Press the buttons 'configure' and then 'generate'.

You should now have a Visual Studio project 'defaultProject.sln' in the build folder.
Open the .sln with Visual Studio 2022 or later, right-click the 'DefaultProject' solution, and select 'Set as Startup Project'.
You can now build and run the code with the play button or 'ctrl + F5'.



How to compile (Ubuntu):
In a terminal navigate to the project you want to compile, e.g. 'home/<username>/Documents/Ember/defaultProject'.
Create a build folder:      mkdir build
navigate into the it:       cd build
run cmake:                  cmake ..
run make:                   make (optional: make -j, for multi core compilation)
navigate to bin:            cd ../bin/<buildType>
execute the project:        ./<Project Name>
Alternatively you can setup visual studio or any IDE of your liking to compile/run the cmake project.



Useful Information:
Device Info: https://vulkan.gpuinfo.org/
Vulkan documentation: https://vulkan.lunarg.com/doc/sdk