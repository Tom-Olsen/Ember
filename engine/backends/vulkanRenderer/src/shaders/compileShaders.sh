#!/bin/bash
set -e

# Abort if VULKAN_SDK is not set
if [ -z "$VULKAN_SDK" ]; then
    exit 1
fi

DXC="$VULKAN_SDK/bin/dxc"

for f in *.comp.hlsl; do
    [ -e "$f" ] || continue  # skip if no matches
    "$DXC" -spirv -T cs_6_0 -E main "$f" -Fo "${f%.hlsl}.spv"
done