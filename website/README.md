# Ember website

All website sources, configuration, dependencies, and local output live in this directory. The GitHub Pages workflow is in `../.github/workflows/docs.yml`, where GitHub Actions requires it.

## Structure

- `mkwebsite.yml`: Material for MkDocs configuration; pass it explicitly with `--config-file`.
- `preview.sh`: starts the live preview and opens Firefox.
- `requirements.txt`: Python dependencies.
- `content/`: Home, Showcase, Documentation, and shared page assets.
- `overrides/`: custom theme templates, separate from published content.
- `.venv/`: ignored local Python environment.
- `site/`: ignored generated website output.

## Preview

From the repository root, run:

```sh
./website/preview.sh
```

Or run `./preview.sh` from this directory. The script works from any working directory, uses `.venv/` when available, and forwards additional MkDocs serve options. Stop the preview with **Ctrl+C** before restarting it. For another port, use `./website/preview.sh --dev-addr 127.0.0.1:8002` from the repository root.

## First-time setup

From the repository root:

```sh
python3 -m venv website/.venv
website/.venv/bin/python -m pip install -r website/requirements.txt
```

No environment activation is required by the preview script.

## Build

From the repository root:

```sh
website/.venv/bin/python -m mkdocs build --strict --config-file website/mkwebsite.yml
```

The output is `website/site/`. From this directory, use `.venv/bin/python -m mkdocs build --strict --config-file mkwebsite.yml`.

Public URLs are independent of this source layout. The configuration preserves redirects from the previous page locations.