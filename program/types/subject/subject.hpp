#pragma once

#include <string>

#include "../entry.hpp"
#include "../../utilities/inputHandler/inputHandler.hpp"
#include "../../utilities/common/constants.hpp"

using Utilities::Common::Constants::g_tableSubjects;
namespace Core::Types
{
    struct Subject : public Entry
    {
        std::string name_;
        Subject() : Entry(0, g_tableSubjects) {}
        Subject(uint16_t id, std::string name) : Entry(id, g_tableSubjects), name_(name) {};
    
        inline std::map<std::string, std::string> getAttrs() const override { return {{"name", name_}};}
        inline std::string toString() const override { return std::to_string(id_) + ". " + name_; }
        inline std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override { name_ = Utilities::InputHandler::getAttrAsStringNonEmpty("Name"); return {std::make_pair("Name", name_)};}  
        Entry& operator=(const Entry& other) override;
    };

} // namespace Core::Types
