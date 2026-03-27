#pragma once

#include "client/renderer/TextureData.hpp"

#include <cstddef>

/// Decode a PNG (from memory) into a TextureData.
/// Returns an empty TextureData on failure.
TextureData loadPngFromMemory(const unsigned char* data, size_t size);

