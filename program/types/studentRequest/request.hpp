#pragma once

#include <inttypes.h>

namespace Core::Types::Request
{
    enum requestStatus
    {
        Approved = 0,
        Denied,
        Pending
    };

    struct Srequest
    {
        uint32_t requestId_;
        uint16_t studentId_;
        uint16_t courseId_;
        requestStatus status_;
    };
} // namespace Core::Types::Request
