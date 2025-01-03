#pragma once

#include <string>

#include "../entry.hpp"

namespace Core::Types
{
    struct Subject : public Entry
    {
        std::string name_;
        Subject(uint16_t id, std::string name) : Entry(id, "Subjects"), name_(name) {};
    
        inline std::map<std::string, std::string> getAttrs() const override { return {{"name", name_}};}
        inline std::string toString() const override { return std::to_string(id_) + ". " + name_; }
    };

} // namespace Core::Types
