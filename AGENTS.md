# Ember Repo Instructions
- This file contains repo-wide instructions for coding agents working in this repository.

## Design Goals
- No hacky solutions. If something needs fixing it must be fixed properly and not with a workaround.

## Coding Style
- Use camelCase for variables/members and PascalCase for functions/methods
- In header files have minimal includes and use forward decleration where possible. An exception to this rule is the #include <vulkan/vulkan.h> in vulkan backend header files.
- In cpp files inclue the header file first, then all custom includes via "..." and then all system includes via "<...>". Both custom and system includes must be sorted alphabetically. Exception to the alphabetical ordering is the include of the .h file into the .cpp file and the parent interface into the .h file which must always be first.
- No new line at end of files.
- GetX should always return a proper valid value, or throw if they cant.
- TryGetX should always return a "invalid" value (nullptr/-1/etc case dependent) on failure that the user can check for success. Log warning on failure.

## Architecture
- Interfaces are reserved for contracts across shared-library boundaries.
- For each class/struct use a .h and .cpp file. Only exceptions are helper structs within other classes/structs which can be defined in the same file as the class/struct they are helping.

## Review Priorities
-

## Communication
- Do not assume anything when implementing a feature. If any design specification is unclear, ask for clarification.

## Skills/Plugins
- when you encounter repatitive tasks that can be optimized via skills and/or plugins give me a suggestion for it.