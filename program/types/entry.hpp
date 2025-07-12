#pragma once

#include <inttypes.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace Core::Types
{

    struct Entry
    {
        uint16_t id_;
        const std::string associatedTable_;

        Entry(const Entry&) = default;

        Entry(std::string table):
            id_(0),
            associatedTable_(table)
        {}

        Entry(uint16_t id, std::string table):
            id_(id),
            associatedTable_(table)
        {}

        virtual ~Entry() {};
        virtual std::string toString() const = 0;

        /// @todo Consider changing this map to std::map<std::string, std::any>
        virtual std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) = 0;

        /// @brief This function shall return all of the SQL related attributes such as
        /// @brief name, related IDs, float values etc. In most cases it will not include "id"
        /// @return Attributes mapped "name" -> "value"
        /// @todo Consider changing this map to std::map<std::string, std::any>
        inline virtual std::map<std::string, std::string> getAttrs() const
        {
            return {
                { "id", std::to_string(id_) }
            };
        };

        /// @brief This function will return a copy of a given struct with it's missing values filled from the provided entry.
        /// @note This function will only pupulate the SQL related members.
        /// @warning Make sure that the passed Entry reference has all the values initialized
        /// @param other Specifies what values will be placed in the missing parts
        virtual std::shared_ptr<Entry> fillGaps(const std::shared_ptr<Entry> other) = 0;

        virtual Entry& operator= (const Entry& other) = 0;
    };

}  // namespace Core::Types
