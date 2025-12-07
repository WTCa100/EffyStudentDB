#pragma once

#include "entry.hpp"
#include "../utilities/common/constants.hpp"
#include "../utilities/logger/logger.hpp"

#include <memory>

namespace Core::Types
{
    struct EntryFactory
    {
    public:
        static std::shared_ptr<Entry> getEntry(const std::string& tableName);
        explicit EntryFactory() {}
        ~EntryFactory() = default;
    };
    

    
} // namespace Core
