#pragma once

#include <inttypes.h>
#include <string>

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

        Srequest() = delete;
        Srequest(uint32_t reqId, uint16_t stdId, uint16_t corId) : Srequest(reqId, stdId, corId, requestStatus::Pending) {}
        Srequest(uint32_t reqId, uint16_t stdId, uint16_t corId, requestStatus status) : requestId_(reqId), studentId_(stdId), courseId_(corId), status_(status) {}
        std::string toString() const;
    };


    std::string statusToString(const requestStatus& status);
} // namespace Core::Types::Request
