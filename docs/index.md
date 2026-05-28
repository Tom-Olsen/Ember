# Ember Engine Docs

This documentation explains engine architecture and design decisions in a visual, repo-local format.

The docs use MkDocs Material with Mermaid diagrams. Files stay readable in the IDE as Markdown, while the rendered site provides navigable diagrams, tables, and search.

## Start Here

- [GUI, Editor, and Input Routing](architecture/gui-editor-engine.md)
- [Window Events](architecture/window-events.md)
- [ADR 0001: GUI Input Capture](adr/0001-gui-editor-input-routing.md)

## Documentation Rules

- Prefer diagrams for cross-system flow.
- Prefer ADRs for decisions that constrain future code.
- Keep code paths explicit so the docs stay easy to verify.
- Keep engine behavior and backend behavior separated when describing ownership.