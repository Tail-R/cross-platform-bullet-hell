#pragma once

#include <string>
#include "frame_enums.hpp"
#include "frame_structs.hpp"

std::string frame_to_json_str(const FrameSnapshot& frame);
void print_frame(const FrameSnapshot& frame);