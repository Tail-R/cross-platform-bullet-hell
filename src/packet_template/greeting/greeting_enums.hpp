#pragma once

#include <cstdint>

enum class GoodByeReasonCode : uint32_t {
    Unknown,
    NormalExit,
    ConnectionError,
    Timeout
};