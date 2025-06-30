#pragma once

#include <cstdint>

enum class GoodByeReasonCode : uint32_t {
    Unknown,
    NormalExit,
    Kicked,
    ConnectionError,
    Timeout
};