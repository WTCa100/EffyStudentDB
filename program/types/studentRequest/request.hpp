#pragma once

#include <inttypes.h>
#include <string>

#include "../entry.hpp"
#include "../../utilities/common/constants.hpp"

using Utilities::Common::Constants::g_tableStudentRequest;
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
        Srequest(uint32_t reqId, uint16_t stdId, uint16_t corId, requestStatus status) : Entry(reqId, g_tableStudentRequest), studentId_(stdId), courseId_(corId), status_(status) {}
        std::string toString() const override;
        std::map<std::string, std::string> getAttrs() const override;
        std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;
        Entry& operator=(const Entry& other) override;
    };


    std::string statusToString(const requestStatus& status);
} // namespace Core::Types::Request
