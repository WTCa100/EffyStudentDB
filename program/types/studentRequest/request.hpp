#pragma once

#include <inttypes.h>
#include <string>

#include "../entry.hpp"

namespace Core::Types::Request
{
    enum requestStatus
    {
        Approved = 0,
        Denied,
        Pending
    };

    struct Srequest : public Entry
    {
        uint16_t studentId_;
        uint16_t courseId_;
        requestStatus status_;

        Srequest() = delete;
        Srequest(uint32_t reqId, uint16_t stdId, uint16_t corId) : Srequest(reqId, stdId, corId, requestStatus::Pending) {}
        Srequest(uint32_t reqId, uint16_t stdId, uint16_t corId, requestStatus status) : Entry(reqId, "StudentRequest"), studentId_(stdId), courseId_(corId), status_(status) {}
        std::string toString() const override;
        std::map<std::string, std::string> getAttrs() const override;
        void userConstruct(bool makeFull = true) override;
    };


    std::string statusToString(const requestStatus& status);
} // namespace Core::Types::Request
