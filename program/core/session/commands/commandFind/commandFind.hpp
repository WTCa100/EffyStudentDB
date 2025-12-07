#pragma once

#include "../ICommand.hpp"
#include "../../../../types/entry.hpp"
#include "../../../displayHelper/displayHelper.hpp"
#include "../../../../utilities/logger/logger.hpp"
#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"

#include <memory>

namespace Core::Commands
{
    class CommandFind : public ICommand
    {
    private:
    std::shared_ptr<Types::Entry> targetEntry_;
    std::unordered_map<std::string, std::string> filterInput_;
    std::shared_ptr<Display::Helper> displayHelper_;
    std::shared_ptr<Utilities::Sql::SqlAdapter>  sqlAdapter_;
    public:
        CommandFind(std::shared_ptr<Utilities::Logger> logger,
                    std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter,
                    std::shared_ptr<Display::Helper> displayHelper,
                    std::unordered_map<std::string, std::string> filterInput,
                    std::shared_ptr<Types::Entry> targetEntry) : 
                        ICommand(logger),
                        targetEntry_(targetEntry),
                        filterInput_(filterInput),
                        displayHelper_(displayHelper),
                        sqlAdapter_(sqlAdapter) {}
        bool exec() override;
        inline std::string name() const override { return "find"; }
        ~CommandFind() = default;
    };
    
} // namespace Core::Commands
