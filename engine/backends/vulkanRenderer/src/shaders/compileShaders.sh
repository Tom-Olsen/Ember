#!/bin/bash
set -e

# Abort if VULKAN_SDK is not set
if [ -z "$VULKAN_SDK" ]; then
    exit 1
fi

DXC="$VULKAN_SDK/bin/dxc"

# Compute shaders
for f in *.comp.hlsl; do
    [ -e "$f" ] || continue
    "$DXC" -spirv -T cs_6_0 -E main "$f" -Fo "${f%.hlsl}.spv"
done

# Vertex shaders
for f in *.vert.hlsl; do
    [ -e "$f" ] || continue
    "$DXC" -spirv -T vs_6_0 -E main "$f" -Fo "${f%.hlsl}.spv"
done

# Fragment shaders
for f in *.frag.hlsl; do
    [ -e "$f" ] || continue
    "$DXC" -spirv -T ps_6_0 -E main "$f" -Fo "${f%.hlsl}.spv"
done