#pragma once

#include "../../utilities/common/constants.hpp"
#include "../entry.hpp"

#include <inttypes.h>
#include <string>

using Utilities::Common::Constants::g_inputMissingValue;
using Utilities::Common::Constants::g_tableStudentRequest;

namespace Core::Types::Request
{
    enum requestStatus
    {
        Approved = 0,
        Denied,
        Pending,
        Unknown
    };

    struct Srequest : public Entry
    {
        // SQL attribute members
        uint16_t studentId_;
        uint16_t courseId_;
        requestStatus status_;

        Srequest():
            Entry(0, g_tableStudentRequest),
            studentId_(0),
            courseId_(0),
            status_(requestStatus::Unknown)
        {}

        Srequest(uint32_t reqId, uint16_t stdId, uint16_t corId):
            Srequest(reqId, stdId, corId, requestStatus::Pending)
        {}

        Srequest(uint32_t reqId, uint16_t stdId, uint16_t corId, requestStatus status):
            Entry(reqId, g_tableStudentRequest),
            studentId_(stdId),
            courseId_(corId),
            status_(status)
        {}

        std::string toString() const override;
        std::map<std::string, std::string> getAttrs() const override;
        std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;
        Entry& operator= (const Entry& other) override;
        std::shared_ptr<Entry> fillGaps(const std::shared_ptr<Entry> other) override;
    };

    std::string statusToString(const requestStatus& status);
}  // namespace Core::Types::Request
