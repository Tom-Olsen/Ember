# Ember Repo Instructions
- This file contains repo-wide instructions for coding agents working in this repository.

## Design Goals
- No hacky solutions. If something needs fixing it must be fixed properly and not with a workaround.

## Coding Style
- Use camelCase for variables/members and PascalCase for functions/methods
- In header files have minimal includes and use forward decleration where possible. An exception to this rule is the #include <vulkan/vulkan.h> in vulkan backend header files.
- In cpp files inclue the header file first, then all custom includes via "..." and then all system includes via "<...>". Both custom and system includes must be sorted alphabetically. Exception to the alphabetical ordering is the include of the .h file into the .cpp file and the parent interface into the .h file which must always be first.
- No new line at end of files.
- In classes/structs the order is always:
	private: // Structs/Enums/...:
	protected: // Structs/Enums/...:
	public: // Structs/Enums/...:
	private: // Members:
	protected: // Members:
	public: // Members:
	public: // Methods:
	protected: // Methods:
	private: // Methods:
	only those that have entries should be listed. If a section is empty it should be omitted.
- The order of methods in .h and .cpp files should be the same. The .cpp must always contain the same header comments above the methods as the .h file.
- GetX should always return a proper valid value, or throw if they cant.
- TryGetX should always return a "invalid" value (nullptr/-1/etc case dependent) on failure that the user can check for success. Log warning on failure.

## Architecture
- For each class/struct use a .h and .cpp file. Only exceptions are helper structs within other classes/structs which can be defined in the same file as the class/struct they are helping.
- no free namespace helper functions in .cpp files. Such helpers should be proper private methods.

## Communication
- Do not assume anything when implementing a feature. If any design specification is unclear, ask for clarification.

## Skills/Plugins
- when you encounter repatitive tasks that can be optimized via skills and/or plugins give me a suggestion for it.

## Project structure:
- Whenever you encounter something that doesnt fit this description let me know so i can update it.
- `engine/common/`: Backend-independent shared enums and data structures used across interfaces, core, and backends.
- `engine/interfaces/`: Abstract engine boundaries. Renderer contracts are in `renderer/`, with separate GUI and window interfaces in `gui/` and `window/`.
- `engine/core/src/`: High-level engine implementation and orchestration. Look in `gpuResources/` for frontend resource wrappers, `managers/` for resource ownership/loading, `renderer/`, `window/`, and `gui/` for subsystem facades, `eventSystem/` for events, `physics/` for physics integration, `editor/` for core editor hooks, and `utility/` for reusable engine algorithms.
- `engine/backends/`: Concrete implementations of the engine interfaces:
  - `vulkanRenderer/src/`: Vulkan renderer. Its main areas are `context/`, `renderer/`, `renderPasses/`, `pipelines/`, `drawCalls/`, `compute/`, `descriptorSetLayouts/`, `pushConstants/`, `gpuResources/`, `gpuResourcePools/`, `managers/`, and `utility/`.
  - `sdlWindow/src/` and `nullWindow/src/`: Interactive SDL and headless window implementations.
  - `imGuiSdlVulkan/src/` and `nullGui/src/`: Interactive ImGui and headless GUI implementations.
- `engine/applications/`: Preconfigured engine compositions used by projects. `emberGameApp/` wires Vulkan + SDL + ImGui, `emberGameDebugApp/` adds editor/debug facilities, `emberEditorApp/` provides the editor application, and `emberHeadlessApp/` wires Vulkan + null window/GUI. Each application exposes its setup through `src/emberEngine.h` and `src/application.*`.
- `engine/libs/`: Reusable libraries, each with its own `CMakeLists.txt` and `src/`: `assetLoader` (image/material/mesh assets), `bufferLayout`, `dataStructures`, `editor`, `entityComponentSystem`, `logger`, `math`, `spirvReflect`, `taskSystem`, and `vulkanUtility`. Library-specific tests live in the corresponding `unitTests/` directory.
- `engine/shaders/`: Engine HLSL and shader compilation CMake. Stage sources are under `src/{vertex,fragment,compute}/`, shared HLSL includes under `src/includes/`, C++/HLSL shared definitions under `src/includesCppHlsl/`, and material descriptions under `materialAssets/`.
- `engine/resources/`: Runtime meshes and textures. Treat binary assets as data, not source.
- `engine/extern/`: Vendored third-party dependencies (SDL, ImGui, EnTT, GoogleTest, SPIRV-Reflect, etc.). Do not search or modify this tree unless the task concerns a dependency.
- `engine/cmake/`: Shared engine CMake configuration, currently Vulkan options.
- `projects/defaultProject/`: Main example/editor executable; custom components are in `src/components/` and scenes in `src/scenes/`.
- `projects/fluidDynamics/`: SPH fluid project. CPU/GPU 2D implementations are in `src/2d/`, 3D GPU code in `src/3d/`, spatial hash grids in `src/utility/`, project shaders in `shaders/`, research notes in `docs/`, and tests in `unitTests/`.
- `projects/unitTests/`: Project-level GPU sort and compute shader tests using the game application stack.
- `projects/plotting/`: Small plotting/graphics experiment; much of its CMake setup is currently disabled.
- `unitTests/`: Top-level test aggregation CMake plus `runAllTests.sh` and `runAllTests.bat`.
- `docs/`, root `readme.txt`, `ToDo*.txt`: Project documentation, design notes, and planned work.
- Generated output is commonly stored in `build/` and backend `bin/` directories. Exclude these directories from source searches unless investigating build artifacts.

## Documentation
- The `docs/` directory contains Ember's standalone documentation website. Its purpose is to explain the engine's big-picture architecture, public API design, and interactions between systems. Portfolio-oriented feature highlights and media belong on the separate portfolio website, which links to both this documentation site and the Ember repository.
- The website is built with Material for MkDocs. `mkdocs.yml` defines site metadata, theme settings, Markdown extensions, and navigation; documentation pages are Markdown files below `docs/`.
- `docs/index.md` is the landing page. Add new pages below `docs/` and register them in the `nav` section of `mkdocs.yml` so the site hierarchy remains explicit.
- Run `mkdocs serve` from the repository root for a local preview. Run `mkdocs build --strict` before publishing to catch invalid configuration, links, and navigation entries.
- `.github/workflows/docs.yml` builds and deploys the site through GitHub Pages when documentation-related files are pushed to `master`. The default public URL is `https://tom-olsen.github.io/Ember/`; GitHub repository settings may replace it with a custom domain.
- Keep documentation conceptual and task-oriented. Prefer diagrams and focused examples where they clarify system relationships; do not duplicate implementation details that are clearer in the source code.