#include <optional>

#include "wsManager.hpp"
#include "constants.hpp"
#include "../common/stringManip.hpp"

namespace Utilities
{
    WsManager::WsManager(std::string workingDir) : workingDir_(std::filesystem::path(workingDir))
    {
        dManager_ = std::make_unique<Workspace::DirectoryManager>(workingDir_);
        fManager_ = std::make_unique<Workspace::FileManager>(workingDir_);
        sManager_ = std::make_unique<Workspace::SqlManager>(fileDatabaseSubdir + "/" + fileDatabase);

        if(isInitializationNeeded())
        {
            dManager_->createDirectory(directoryLogs);
            logger_ = std::make_shared<Logger>(directoryLogs);        
            LOG((*logger_), "Initialization required.");
            initializeDatabase();
        }
        else
        {
            logger_ = std::make_shared<Logger>(directoryLogs);  
            LOG((*logger_), "Initialization not required. Will extract data from existing files");
            auto schemaPtr = fManager_->getFile(fileBase, fileBaseSubdir);
            sManager_->openDb();
            sManager_->initialTablesLoad(*schemaPtr);
        }

        LOG((*logger_), "WsManager :ctor: specialized - with working directory: ", workingDir_);
        std::cout << "WsManager :ctor: specialized - with working directory: " << workingDir_ << "\n";
    }

    void WsManager::initializeDatabase()
    {
        // First - handle directories
        createDirectory(directoryDatabase);
        createDirectory(directorySchemas, directorySchemasSubdir);

        // Second - handle files 
        if(createFile(fileBase, fileBaseSubdir))
        {
            if(!createInitialSchema())
            {
                LOG((*logger_), "Unable to create initial schema file!");
                throw std::runtime_error("Unable to create initial schema file!");
            }

            if(!fileExists(fileDatabase, fileDatabaseSubdir))
            {
                if(!createFile(fileDatabase, fileDatabaseSubdir))
                {
                    LOG((*logger_), "Unable to create initial database file!");
                    throw std::runtime_error("Unable to create initial database file!");
                }

                sManager_->openDb();
                // Get all schemas into the .db
                if(!sManager_->moveSchemasToDatabase())
                {
                    LOG((*logger_), "Could not add inital schemas into the database!");
                }
                else
                {
                    LOG((*logger_), "All schemas have been moved into the database!");
                }
            }
        }
        LOG((*logger_), "Initial start-up done! Ready to operate.");
    }

    bool WsManager::createInitialSchema()
    {
        LOG((*logger_), "Creating the initial schema file with default tables");
        std::unique_ptr<std::fstream> schemaPtr = fManager_->getFile(fileBase, fileBaseSubdir);
        if(!schemaPtr->is_open())
        {
            LOG((*logger_), "Could not open file: ", fileDatabase, " with subdir: ", fileDatabaseSubdir);
            return false;
        }

        // Fill the base schema file
        Table schoolsTbl       = defaultSchoolsTable();
        Table studentsTbl      = defaultStudentsTable();
        Table subjectsTbl      = defaultSubjectsTable();
        Table gradesTbl        = defaultGradesTable();
        Table coursesTbl       = defaultCoursesTable();
        Table subjectWeightTbl = defaultSubjectToCourseWeightTable();
        Table studentReqTbl    = defaultStudentRequestTable();

        *schemaPtr << "-- Effy.db - this file has been generated automatically\n";
        *schemaPtr << "-- Do not modify it!\n";
        *schemaPtr << schoolsTbl.makeFormula();
        *schemaPtr << studentsTbl.makeFormula(); 
        *schemaPtr << subjectsTbl.makeFormula(); 
        *schemaPtr << gradesTbl.makeFormula(); 
        *schemaPtr << coursesTbl.makeFormula();
        *schemaPtr << subjectWeightTbl.makeFormula();
        *schemaPtr << studentReqTbl.makeFormula();

        // To make the boot up faster, insert the intial tables into the sql manager
        sManager_->addTable(schoolsTbl);
        sManager_->addTable(studentsTbl);
        sManager_->addTable(subjectsTbl);
        sManager_->addTable(gradesTbl);
        sManager_->addTable(coursesTbl);
        sManager_->addTable(subjectWeightTbl);
        sManager_->addTable(studentReqTbl);

        LOG((*logger_), "Initial schema was written into ", "base.sql");
        schemaPtr->close();
        return true;
    }


    bool WsManager::isInitializationNeeded()
    {
        return(!dManager_->exist(directoryLogs) ||
               !dManager_->exist(directoryDatabase) ||
               !dManager_->exist(directorySchemas, directorySchemasSubdir) ||
               !fManager_->exist(fileBase, fileBaseSubdir) ||
               !fManager_->exist(fileDatabase, fileDatabaseSubdir));
    }

    bool WsManager::createFile(std::string name, std::optional<std::filesystem::path> subPath)
    {
        LOG((*logger_), "fileName=", name, " subPath=", subPath.has_value() ? subPath.value() : "nullopt");
        // Check if file exists
        // If file exists ask to override
        if(fManager_->exist(name, subPath))
        {
            // For now just abort - don't override
            std::cout << "File: \"" << name << "\" exists at: " << workingDir_.string();
            if(subPath.has_value())
            {
                std::cout << "/" << subPath.value().string();
            } 
            std::cout << "\n";
            std::cout << "Skipping...\n";
            return false;
        }
        // If override - override - return true
        // If not exit - return false
        std::cout << "Creating file: \"" << name << "\" at: " << workingDir_.string();
        if(subPath.has_value())
        {
            std::cout << "/" << subPath.value().string();
        }
        std::cout << "\n";

        // For now - will make this prettier later
        bool rc = fManager_->createFile(name, subPath);
        return rc;
    }

    bool WsManager::fileExists(std::string fileName, std::optional<std::filesystem::path> subPath)
    {
        std::cout << "Looking for the file " << fileName << "\n";
        bool rc = fManager_->exist(fileName, subPath);

        std::cout << (rc ? ("File: " + fileName + " was found!\n") 
                        : ("Could not find file: " + fileName + "\n"));
        return rc;
    }


    bool WsManager::deleteFile(std::string fileName, std::optional<std::filesystem::path> subPath, bool prompt)
    {
        LOG((*logger_), "Deleting file: fileName=", fileName, " subPath=",(subPath.has_value() ? subPath.value() : "nullopt"), "prompt=", (prompt ? "true" : "false") );
        // First check if exists
        if(prompt)
        {
            std::cout << "This function will permamently delete given file. This cannot be undone!\n";
            std::cout << "Are you sure you want to continue and delete " << fileName << "?\n";
            std::string tmp;
            std::getline(std::cin, tmp);
            if(tolower(tmp[0]) != 'y')
            {
                std::cout << "Skipping...\n";
                return false;
            }
        }

        bool rc = fManager_->deleteFile(fileName, subPath);
        return rc;

    }


    bool WsManager::createDirectory(std::string directoryName, std::optional<std::filesystem::path> subPath)
    {
        std::cout << "Creating directory: " << directoryName << "\n";
        LOG((*logger_), "Creating directory: directoryName=", directoryName, " subPath=", (subPath.has_value() ? subPath.value() : "nullopt"));
        // Check for folder
        bool isCreated;
        if((isCreated = dManager_->exist(directoryName, subPath)))
        {
            std::cout << "Folder already exists! Skipping...\n";
            return isCreated;
        }

        isCreated = dManager_->createDirectory(directoryName, subPath);
        if(isCreated)
        {
            LOG((*logger_), "Directory created without any issues!");
        }
        else
        {
            LOG((*logger_), "Could not create a given directory")
        }

        return isCreated;
    }

    bool WsManager::directoryExists(std::string directoryName, std::optional<std::filesystem::path> subPath)
    {
        std::cout << "Looking for the directory: " << directoryName << "...\n";
        bool rc = dManager_->exist(directoryName, subPath);
    
        if(rc)
        {
            std::cout << "File: " + directoryName + " was found!\n";
            LOG((*logger_), "Directory ", directoryName, "exists");
        }
        else
        {
            std::cout << "Could not find file: " + directoryName + "\n";
            LOG((*logger_), "Directory ", directoryName, " does not exists");
        }
        return rc;
    }

    bool WsManager::deleteDirectory(std::string directoryName, std::optional<std::filesystem::path> subPath, bool prompt)
    {
        LOG((*logger_), "Attempting to delete directory: name=", directoryName, "subpath=", subPath.has_value() ? subPath.value().string() : "nullopt");
        // Check if a file exists - if there is no file, there is no need on doing anythin
        if(!dManager_->exist(directoryName, subPath))
        {
            std::cout << "No such file in directory! Skipping...\n";
            return true;
        }

        if(prompt)
        {
            LOG((*logger_), "Deletion confirmation prompt triggered");
            std::cout << "This function will permamently delete given folder and it's content. This cannot be undone!\n";
            std::cout << "Are you sure you want to continue and delete " << directoryName << "?\n";
            std::string tmp;
            std::getline(std::cin, tmp);
            if(tolower(tmp[0]) != 'y')
            {
                std::cout << "Skipping...\n";
                return false;
            }
        }

        std::cout << "Attempting to remove " << directoryName << "...\n";
        uint16_t tryCount = 0;
        bool hasDeleted = false;
        do
        {
            ++tryCount;
            hasDeleted = dManager_->deleteDirectory(directoryName, subPath);
            LOG((*logger_), "Deletion attempt: ", tryCount, " result=", hasDeleted ? "successful" : "unsuccessful");
        } while (!hasDeleted && tryCount <= 3);

        std::cout << (hasDeleted ? ("Directory " + directoryName + " was deleted\n") : ("Could not delete directory " + directoryName + ".\n"));
        return hasDeleted;
    }

    std::vector<Core::Types::School> WsManager::getSchools()
    {
        std::vector<Core::Types::School> schools;
        LOG((*logger_), "Fetching schools from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("Schools");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called schools, but got no entries!");
            std::cout << "No schools! Either fail or sql has no schools\n";
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " school entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedSchool = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0)NAME | (1)ID
            schools.push_back(Core::Types::School{static_cast<uint16_t>(std::stoul(tokenizedSchool.at(1))), tokenizedSchool.at(0), {}});
        }
        LOG((*logger_), "Schools tokenized and pushed into the list. Final vector size = ", schools.size(), " Raw entries size = ", rawEntries.size());
        return schools;
    }

    std::vector<Core::Types::Student> WsManager::getStudents()
    {
        std::vector<Core::Types::Student> students;
        LOG((*logger_), "Fetching students from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("Students");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called students, but got no entries!");
            std::cout << "No students! Either fail or sql has no students\n";
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " student entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedStudent = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0) EMAIL | (1)LASTNAME | (2) SECONDNAME | (3) FIRSTNAME | (4) SCHOOLID | (5) ID
            // Translates to:
            // (0) ID | (1) FIRSTNAME | (2) SECONDNAME | (3) LASTNAME | (4) EMAIL | (5) SCHOOLID 
            std::optional<std::string> secondName = std::nullopt;
            if(tokenizedStudent.at(2) != "NULL") { secondName = tokenizedStudent.at(1); }
            
            Core::Types::Student tmpStudent;
            tmpStudent.id_         = std::stoul(tokenizedStudent.at(5));
            tmpStudent.firstName_  = tokenizedStudent.at(3);
            tmpStudent.secondName_ = secondName;
            tmpStudent.lastName_   = tokenizedStudent.at(1);
            tmpStudent.email_      = tokenizedStudent.at(0);
            tmpStudent.grades_     = {};
            tmpStudent.schoolId_   = std::stoul(tokenizedStudent.at(4));
            students.push_back(tmpStudent);
        }
        LOG((*logger_), "Students tokenized and pushed into the list. Final vector size = ", students.size(), " Raw entries size = ", rawEntries.size());
        return students;
    }
    
    std::vector<Core::Types::Subject> WsManager::getSubjects()
    {
        std::vector<Core::Types::Subject> subjects;
        LOG((*logger_), "Fetching subjects from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("Subjects");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called subjects, but got no entries!");
            std::cout << "No subjects! Either fail or sql has no subjects\n";
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " subjects entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedSubjects = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0)NAME | (1)ID
            subjects.push_back(Core::Types::Subject{static_cast<uint16_t>(std::stoul(tokenizedSubjects.at(1))), tokenizedSubjects.at(0)});
        }
        LOG((*logger_), "Subjects tokenized and pushed into the list. Final vector size = ", subjects.size(), " Raw entries size = ", rawEntries.size());
        return subjects;
    }

    std::vector<std::vector<std::string>> WsManager::getGrades()
    {
        LOG((*logger_), "Fetching grades from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("grades");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called grades, but got no entries!");
            std::cout << "No grades! Either fail or sql has no grades\n";
            return {};
        }

        std::vector<std::vector<std::string>> listOfGrades;
        LOG((*logger_), "Got n=", rawEntries.size(), " grades entries");
        for(auto e : rawEntries)
        {
            listOfGrades.push_back(Utilities::Common::tokenize(e, '|'));
        }
        LOG((*logger_), "Grades tokenized and pushed into the list. Final vector size = ", listOfGrades.size(), " Raw entries size = ", rawEntries.size());
        return listOfGrades;
    }

    uint16_t WsManager::getLatestIdFromTable(std::string tblName)
    {
        LOG((*logger_), "Getting latest id from ", tblName);
        std::vector<std::string> result = sManager_->getEntriesFromTable(tblName, {"MAX(id)"});
        // Check if it's empty - such scenario would probably never occur?
        LOG((*logger_), "Max ID from table = ", std::stoul(result.at(0)));
        return static_cast<uint16_t>(std::stoul(result.at(0)));
    }

    bool WsManager::addSchool(Core::Types::School& newSchool)
    {
        LOG((*logger_), "Adding new school \"", newSchool.name_, "\"");
        // Check if given entry exists
        // if(sManager_->) Check if entry already exists
        // Prepare atributes
        Table targetTable = sManager_->getTableSchema("Schools");
        if(sManager_->addEntryToTable(targetTable.getName(),
            { std::make_pair(targetTable.getAttributeByName("name") , newSchool.name_)}))
        {
            newSchool.id_ = getLatestIdFromTable(targetTable.getName());
            LOG((*logger_), "New school added. Name=", newSchool.name_, " id=", newSchool.id_);
            return true;
        }
    
        LOG((*logger_), "Could not add given school into the database. Name=", newSchool.name_);
        return false;
    }

    bool WsManager::removeSchool(const Core::Types::School& removeSchool)
    {
        LOG((*logger_), "Attempting to remove school \"", removeSchool.name_, "\"");
        // TODO Check if exists - if not, nothing to delete and return true
        sManager_->removeEntryFromTable("Schools", removeSchool.id_);\
        // TODO verify
        return true;
    }

    bool WsManager::addStudent(Core::Types::Student& newStudent)
    {
        LOG((*logger_), "Adding new student \"", newStudent.email_, "\"");
        // Check if given entry exists
        // if(sManager_->) Check if entry already exists
        // Prepare atributes
        Table targetTable = sManager_->getTableSchema("Students");
        if(sManager_->addEntryToTable(targetTable.getName(),
            { std::make_pair(targetTable.getAttributeByName("firstName") , newStudent.firstName_),
              std::make_pair(targetTable.getAttributeByName("secondName"), (newStudent.secondName_ ? newStudent.secondName_.value() : "" )),
              std::make_pair(targetTable.getAttributeByName("lastName"), newStudent.lastName_),
              std::make_pair(targetTable.getAttributeByName("email"), newStudent.email_),
              std::make_pair(targetTable.getAttributeByName("schoolId"), std::to_string(newStudent.schoolId_))}))
        {
            newStudent.id_ = getLatestIdFromTable(targetTable.getName());
            LOG((*logger_), "New student added. Email=", newStudent.email_, " id=", newStudent.id_);
            return true;
        }
        LOG((*logger_), "Could not add given student into the database. Name=", newStudent.firstName_, " ", newStudent.lastName_, " email=", newStudent.email_);
        return false;
    }

    bool WsManager::removeStudent(const Core::Types::Student& targetStudent)
    {
        LOG((*logger_), "Attempting to remove student \"", targetStudent.email_, "\"");
        sManager_->removeEntryFromTable("Students", targetStudent.id_);
        return true;
    }

    bool WsManager::addSubject(Core::Types::Subject& newSubject)
    {
        LOG((*logger_), "Adding new subject \"", newSubject.name_);
        Table targetTable = sManager_->getTableSchema("Subjects");
        if(sManager_->addEntryToTable(targetTable.getName(), 
            {std::make_pair(targetTable.getAttributeByName("name"), newSubject.name_)}))
        {
            newSubject.id_ = getLatestIdFromTable(targetTable.getName());
            LOG((*logger_), "New subject added. Name=", newSubject.name_, " id=", newSubject.id_);
            return true;
        }
        LOG((*logger_), "Could not add given subject into the database. Name=", newSubject.name_);
        return false;
    }

    bool WsManager::removeSubject(const Core::Types::Subject& targetSubject)
    {
        LOG((*logger_), "Attempting to remove subject \"", targetSubject.name_, "\"");
        sManager_->removeEntryFromTable("Subjects", targetSubject.id_);
        return true;
    }

    bool WsManager::addGrade(Core::Types::Student& targetStudent, Core::Types::Subject& targetSubject, const float& grade)
    {
        LOG((*logger_), "Adding new grade from \"", targetSubject.name_, "\" to student ", targetStudent.firstName_ , " ", targetStudent.lastName_);
        Table targetTable = sManager_->getTableSchema("Grades");
        if(sManager_->addEntryToTable(targetTable.getName(),
            {std::make_pair(targetTable.getAttributeByName("grade"),     std::to_string(grade)),
             std::make_pair(targetTable.getAttributeByName("subjectId"), std::to_string(targetSubject.id_)),
             std::make_pair(targetTable.getAttributeByName("studentId"), std::to_string(targetStudent.id_))}))
        {
            LOG((*logger_), "Assigned grade (", grade, ") to a student [", targetStudent.email_, " ", targetStudent.id_, "] from a subject [", targetSubject.name_, " ", targetSubject.id_, "]");
            return true;
        }
        LOG((*logger_), "Could not assign given grade");
        return false;
    }

    bool WsManager::removeGrade(const Core::Types::Student& targetStudent, const Core::Types::Subject& targetSubject)
    {
        LOG((*logger_), "Attempting to remove grade from \"", targetSubject.name_, "\" from student ", targetStudent.firstName_ , " ", targetStudent.lastName_);
        // TODO add handle + add new remove entry from table methods WITH condition not just id
        return true;
    }

}
