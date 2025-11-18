#pragma once

#include "../ICommand.hpp"
#include "../../data/data.hpp"
#include "../../../displayHelper/displayHelper.hpp"
#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"

#include<memory>

namespace Core::Commands
{
    class CommandShow : public ICommand
    {
    private:
        std::shared_ptr<SessionData> sessionData_;
        std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter_;
        std::shared_ptr<Display::Helper> displayHelper_;
        const std::string target_;
        void showEntry(const uint16_t& id) const;
    public:
        bool exec() override;
        inline std::string name() const override { return "show"; }
        CommandShow(std::shared_ptr<Utilities::Logger> logger,
                    std::shared_ptr<SessionData> sesData,
                    std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter,
                    std::shared_ptr<Display::Helper> displayHelper,
                    const std::string& target) : ICommand(logger), 
                                                 sessionData_(sesData), 
                                                 sqlAdapter_(sqlAdapter),
                                                 displayHelper_(displayHelper),
                                                 target_(target) {}
        ~CommandShow() = default;
    };

}