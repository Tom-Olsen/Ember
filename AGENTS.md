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
- `website/`: Project website source, including Overview, Showcase, Documentation, and shared assets.
- Root `readme.txt`, `ToDo*.txt`: Project notes and planned work.
- Generated output is commonly stored in `build/` and backend `bin/` directories. Exclude these directories from source searches unless investigating build artifacts.

## Documentation
- The `website/` directory contains all Ember website source and tooling. The Showcase section owns feature demonstrations and media; Documentation explains engine architecture, public API design, and interactions between systems. The separate portfolio links directly to both sections and the Ember repository. General lessons about algorithms, mathematics, and physics belong in Sparkive.
- The website uses Material for MkDocs. `website/mkwebsite.yml` defines metadata, theme settings, Markdown extensions, and navigation. Always pass `--config-file website/mkwebsite.yml` when invoking MkDocs from the repository root.
- Page sources and shared assets live in `website/content/`. `docs_dir: content`, `site_dir: site`, and `theme.custom_dir: overrides` are resolved relative to `website/mkwebsite.yml`. Templates and tooling sit outside `content/` and are not published as page content.
- Theme templates live in `website/overrides/`, header links and palettes are configured in `website/mkwebsite.yml`, and shared styling lives in `website/content/stylesheets/ember.css`.
- Website sections are `website/content/overview/`, `website/content/showcase/`, and `website/content/documentation/`, each with `index.md`. Renderer guides live in `website/content/documentation/vulkan-renderer/`. Public paths remain `/Ember/overview/`, `/Ember/showcase/`, and `/Ember/documentation/`. Preserve existing redirects in `website/mkwebsite.yml`.
- Each showcase chapter has a camelCase directory, such as `website/content/showcase/fluidDynamics/` or `website/content/showcase/screenSpaceReflections/`. Put its page in `index.md` and its media directly alongside it. Keep the section introduction in `website/content/showcase/index.md`. Register new pages in `nav`.
- Website dependencies are listed in `website/requirements.txt`. Create the local environment with `python3 -m venv website/.venv`, then install with `website/.venv/bin/python -m pip install -r website/requirements.txt`. The environment and generated `website/site/` output are ignored by `website/.gitignore`.
- Run `./website/preview.sh` from the repository root, or `./preview.sh` from `website/`, for a live preview. The script resolves its own directory and configuration. Run `website/.venv/bin/python -m mkdocs build --strict --config-file website/mkwebsite.yml` before publishing.
- `.github/workflows/docs.yml` is the GitHub-required exception to keeping website tooling in `website/`. It deploys GitHub Pages when website files are pushed to `master`. The default public URL is `https://tom-olsen.github.io/Ember/`; repository settings may replace it with a custom domain.
- Keep documentation conceptual and task-oriented. Prefer diagrams and focused examples where they clarify system relationships; do not duplicate implementation details that are clearer in source code.