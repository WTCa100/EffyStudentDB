#pragma once

#include <memory>

#include "../../utilities/workspaceManager/wsManager.hpp"
#include "../../utilities/logger/logger.hpp"
#include "../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../menu/menu.hpp"
#include "data/data.hpp"
#include "action/action.hpp"

using Core::Action;
using Utilities::WsManager;
using Utilities::Sql::SqlAdapter;
using Utilities::Logger;
using Core::Display::Menu;
class Session
{
    private:
    /* - input-handler ptr (a class that will handle the user input)
       - Formular creator (either a class or just a function that handles a formula to a given targeted major)*/
    std::shared_ptr<WsManager> wsMgr_; 
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<SqlAdapter> sAdapter_;
    std::shared_ptr<SessionData> sesData_;
    
    // Consider removing this since we're (for now) only using it in one class
    std::unique_ptr<Menu> display_;


    // Get values from the Database and assign it to the Session Data
    void fetchAll();
    void fetchSchools();
    void fetchStudents();
    void fetchSubjects();
    void fetchGrades();
    void fetchCourses();
    void fetchSrequests();

    public:
    void run();
    bool handleAction(const Action& userAction);
    void onAdd(const std::shared_ptr<Entry> newEntry);
    void onDelete(const std::shared_ptr<Entry> targetEntry);
    void onUpdate(const std::shared_ptr<Entry> oldEntry, const std::shared_ptr<Entry> newEntry);

    std::shared_ptr<Entry> makeConcreteType(const std::string& tableName) const;

    // Updating
    bool updateSchool(School& targetSchool, School& newSchool);
    bool updateCourse(Course& targetCourse, Course& newCourse);

    Session(std::shared_ptr<WsManager> wsMgr);
    ~Session();
};
