#pragma once

#include <inttypes.h>
#include <string>
#include <map>

namespace Core::Types
{
    
    struct Entry
    {
        uint16_t id_;
        std::string associatedTable_;

        Entry(std::string table) : id_(0), associatedTable_(table) {}
        Entry(uint16_t id, std::string table) : id_(id), associatedTable_(table) {}
        virtual ~Entry() {};
        virtual std::string toString() const = 0;
       
        /// @brief This function shall return all of the SQL related attributes such as 
        /// name, related IDs, float values etc. It shall not include id.
        /// @return 
        inline virtual std::map<std::string, std::string> getAttrs() const { return {{"id", std::to_string(id_)}};};
    };
    

} // namespace Core::Types
