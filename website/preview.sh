#!/usr/bin/env bash
set -euo pipefail

websiteDir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd -- "$websiteDir"

pythonBin="$websiteDir/.venv/bin/python"
if [[ ! -x "$pythonBin" ]]; then
    pythonBin="python3"
fi

if ! command -v "$pythonBin" >/dev/null 2>&1 || ! "$pythonBin" -c 'import mkdocs, material, mkdocs_redirects' >/dev/null 2>&1; then
    printf '%s\n' 'Website dependencies are missing. Set them up with:' >&2
    printf '  cd -- %q\n' "$websiteDir" >&2
    printf '%s\n' '  python3 -m venv .venv' '  .venv/bin/python -m pip install -r requirements.txt' >&2
    exit 1
fi

if ! command -v firefox >/dev/null 2>&1; then
    printf '%s\n' 'Firefox was not found. Install it and ensure firefox is on PATH.' >&2
    exit 1
fi

export BROWSER=firefox
exec "$pythonBin" -m mkdocs serve --config-file "$websiteDir/mkwebsite.yml" --open "$@"