# Visualizing the Docs

The documentation is stored as Markdown and rendered as a local MkDocs site.

## One-Time Setup

Create a Python virtual environment and install MkDocs Material:

```bash
python3 -m venv .venv-docs
source .venv-docs/bin/activate
python -m pip install mkdocs-material
```

## Preview Locally

From the repository root:

```bash
source .venv-docs/bin/activate
mkdocs serve
```

Open the local URL printed by MkDocs, usually:

```txt
http://127.0.0.1:8000
```

MkDocs watches the files, so editing Markdown or Mermaid diagrams should update the browser automatically.

## Build Static HTML

To generate a static site:

```bash
source .venv-docs/bin/activate
mkdocs build
```

The output goes to `site/`.