#pragma once

#include <vector>
#include <cstddef>
#include <optional>
#include "frame_template.hpp"

std::optional<std::vector<std::byte>> serialize_frame(const FrameSnapshot& frame);
std::optional<FrameSnapshot> deserialize_frame(const std::vector<std::byte>& bytes);