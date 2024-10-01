#pragma once

#include <memory>

#include "../../utilities/workspaceManager/wsManager.hpp"
#include "../../utilities/logger/logger.hpp"
#include "data/data.hpp"

using Utilities::WsManager;
using Utilities::Logger;
class Session
{
    private:
    /* - data (a data class that will hold info about all DB data + recent changes made) 
       - menu (a class that will be in charge of displaying the UI for the user)
       - input-handler ptr (a class that will handle the user input)
       - Workspace Manager ptr (a class that is in charge of delegating jobs to respected sub managers)*/
    std::shared_ptr<WsManager> wsMgr_; 
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<SessionData> sesData_;

    void fetchAll();
    void fetchSchools();
    void fetchStudents();
    void fetchSubjects();
    void fetchGrades();

    public:
    Session(std::shared_ptr<WsManager> wsMgr, std::shared_ptr<Logger> logger);
    ~Session();
};
