#pragma once

#include <string>
#include "frame/frame_enums.hpp"
#include "frame/frame_structs.hpp"

std::string frame_to_json_str(const FrameSnapshot& frame);
void print_frame(const FrameSnapshot& frame);