#pragma once

#include "../../utilities/logger/logger.hpp"
#include "../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../../utilities/workspaceManager/wsManager.hpp"
#include "../menu/menu.hpp"
#include "action/action.hpp"
#include "data/data.hpp"

#include <memory>
#include <type_traits>

using Core::Action;
using Core::Display::Menu;
using Utilities::Logger;
using Utilities::Sql::SqlAdapter;
using Utilities::WsManager;

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
    void fetchCourseSubjectWeight();
    void fetchCourses();
    void fetchSrequests();

    void dropAll();

    // Some common procedures
    void deleteCourseSubjectWeight(const std::shared_ptr<CourseSubjectWeight> targetWeight);
    void deleteGrade(std::shared_ptr<Grade> targetGrade);

  public:
    void run();
    bool handleAction(const Action& userAction);
    void onAdd(const std::shared_ptr<Entry> newEntry);
    void onDelete(const std::shared_ptr<Entry> targetEntry);
    void onUpdate(std::shared_ptr<Entry> oldEntry, const std::shared_ptr<Entry> newEntry);

    std::shared_ptr<Entry> makeConcreteType(const std::string& tableName) const;

    template<typename T, typename U = Entry>
    void updateSingleEntry(std::shared_ptr<U> entryOld, const std::shared_ptr<U> entryNew)
    {
        std::shared_ptr<T> concreteOld = std::dynamic_pointer_cast<T>(entryOld);
        std::shared_ptr<T> concreteNew = std::dynamic_pointer_cast<T>(entryNew);
        LOG((*logger_), "Entry update trigger. Id= ", entryOld->id_);
        *concreteOld = *concreteNew->fillGaps(concreteOld);
        LOG((*logger_), "Entry update completed.")
    }

    Session(std::shared_ptr<WsManager> wsMgr);
    ~Session();
};
