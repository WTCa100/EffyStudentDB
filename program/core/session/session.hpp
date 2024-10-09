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
    /* - input-handler ptr (a class that will handle the user input)
       - Formular creator (either a class or just a function that handles a formula to a given targeted major)*/
    std::shared_ptr<WsManager> wsMgr_; 
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<SessionData> sesData_;


    // Get values from the Database and assign it to the Session Data
    void fetchAll();
    void fetchSchools();
    void fetchStudents();
    void fetchSubjects();
    void fetchGrades();

    public:

    // Adding
    bool addSchool(School newSchool);
    bool addStudent(Student newStudent);
    bool addSubject(Subject newSubject);
    bool addGrade(Subject targetSubject, Student targetStudent);

    // Removing
    bool removeSchool(School targetSchool);
    bool removeStudent(Student targetStudent);
    bool removeSubject(Subject targetSubject);
    bool removeGrade(Subject targetSubject, Student targetStudent);

    Session(std::shared_ptr<WsManager> wsMgr, std::shared_ptr<Logger> logger);
    ~Session();
};
