#include <optional>

#include "wsManager.hpp"
#include "constants.hpp"
#include "../common/stringManip.hpp"

namespace Utilities
{
    WsManager::WsManager(std::string workingDir) : workingDir_(std::filesystem::path(workingDir))
    {
        // First handle non-logging objects
        dManager_ = std::make_unique<Workspace::DirectoryManager>(workingDir_);
        fManager_ = std::make_unique<Workspace::FileManager>(workingDir_);
        
        // Handle log folder separately
        if(!dManager_->exist(directoryLogs))
        {
            dManager_->createDirectory(directoryLogs);
        }

        // Handle logger and logging objects
        logger_ = std::make_shared<Logger>(directoryLogs);
        sManager_ = std::make_unique<Utilities::Sql::SqlManager>(logger_, fileDatabaseSubdir + "/" + fileDatabase);

        if(isInitializationNeeded())
        {
            LOG((*logger_), "Initialization required.");
            initializeDatabase();
        }
        else
        {
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
        Types::Table schoolsTbl       = Types::defaultSchoolsTable();
        Types::Table studentsTbl      = Types::defaultStudentsTable();
        Types::Table subjectsTbl      = Types::defaultSubjectsTable();
        Types::Table gradesTbl        = Types::defaultGradesTable();
        Types::Table coursesTbl       = Types::defaultCoursesTable();
        Types::Table subjectWeightTbl = Types::defaultSubjectToCourseWeightTable();
        Types::Table studentReqTbl    = Types::defaultStudentRequestTable();

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
        return(!dManager_->exist(directoryDatabase) ||
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

}
