# Ember Repo Instructions
This file contains repo-wide instructions for coding agents working in this repository.
Add new rules here as the project evolves.

## Design Goals
- Engine is inspired by unity. Many systems should be designed analogously to unity's systems.
- No hacky solutions. If something needs fixing it must be fixed properly and not with a workaround.

## Coding Style
- Use camelCase for variables/members and PascalCase for functions/methods
- In header files have minimal includes and use forward decleration where possible.
- In cpp files inclue the header file first, then all custom includes via "..." and then all system includes via "<...>". Both custom and system includes must be sorted alphabetically.
- No new line at end of files.

## Architecture
- Interfaces are reserved for contracts across shared-library boundaries.
- For each class/struct use a .h and .cpp file. Only exceptions are helper structs within other classes/structs which can be defined in the same file as the class/struct they are helping.

## Review Priorities
-

## Communication
- Do not assume anything when implementing a feature. If any design specification is unclear, ask for clarification.