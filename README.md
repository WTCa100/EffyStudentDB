# EffyStudentDB
EffyStudentDb is a student management application that allows the user to quickly resolve student requests to join a given course with given grades, based on the calculation formula that every course has specified.

## Dependencies
EffyStudentDb uses a non-built-in C++ library called `SQLite3`. Before compiling, ensure that `SQLite3` is present in your working environment both as a library and a service.

## Compiling
To compile the program, you need to have `CMake` and execute it by pointing to `CMakeFile.txt` located under `program/` as a project source.
```bash
# Example usage
cmake -S ./program -B ./build
```
<strong>Note:</strong> Ensure that you have at least `CMake` with at least `3.10` version.

# Running
## Fresh start
EffystudentDb operates on a local `sqlite3` server instance, and it will generate it the moment the program detects that at least one of its core functionality directories does not exist. 
This is how a valid binary structure would look:
```json
{
"database/":
  {
  "schemas/":
    {
      "base.sql"
    }
  "Effy.db"
  }
"logs/":
  {
    // This folder stores debug logs
  }
}
```
Under the `base.sql`, a one-time generated schema file will contain all the information for:
- The application to understand how a given schema should be handled while creating the `Effy.db` file
- The user to quickly investigate how the server schema is laid out.
Snippet of that file:
```sql
-- Effy.db - this file has been generated automatically
-- Do not modify it!
CREATE TABLE SCHOOLS(
name TEXT UNIQUE NOT NULL ,
id INTEGER PRIMARY KEY 
);
CREATE TABLE STUDENTS(
-- More table definitons below...
```
`Effy.db` is created and initialized as a structured empty database.
## Funcionalities
### Database management
Currently, the user can only manage what is inside the database by entering the database management screen. <br>
There are a few management options present in this screen. Each of them triggers additional functionalities, and most of them prompt the user to provide more information regarding which entries shall be affected by a given command and in what manner. The following list will give an exhaustive overview of how each command is handled in a high-level concept. <br>
- **ADD** - allows the user to *add* entry to a given *table* <br>
The user will be prompted to provide new information to create an entry to the database at *table* name that was provided <br>
- **REMOVE** - allows the user to *remove* entry from a given *table* <br>
The user will be prompted to provide filter information, on which entries shall be deleted. Filtered options will be displayed on the screen, and a final approval prompt will be  displayed. If the user agrees, everything listed will be deleted. <br>
- **EDIT** - allows the user to *edit* entry from a given *table* with new values <br>
The user will be prompted to provide filter information, on which entries shall be modified, then a new prompt will be displayed, allowing the user to provide new information to those entries. <br> **NOTE** If a given column does not need to be modified, it can be left empty, as the program will automatically assign the original values to that column, and change only the ones that have a different value present. <br>
- **ASSIGN** - allows the user to *assign* a student to a given *course* <br>
The user must include a valid *courseId* and *studentId*, and it will insert that student into the course with the maximum possible score that can be obtained. <br>
- **DROP** - allows the user to *drop* a student from a given *course* <br>
The user must include a valid *courseId* and *studentId*, and it will remove that student from the course attendees. <br>
- **CLOSE** - allows the user to *close* a given course from fruther requests <br>
The user must include a valid *courseId*, and it will make students unable to make further requests to that course. <br>
- **OPEN** - allows the user to *open* a given course for fruther requests <br>
The user must include a valid *courseId*, and it will enable students to make requests to that course. <br>
- **FIND** - allows the user to *find* an entry with given values <br>
The user will be prompted to enter filter information for that specific *table* name. <br>
- **LIST** - Provides the user with a full list of all possible table names <br>
- **SHOW** - Displays to the user every entry in a given table with no filters. (equivalent to an empty **FIND**)<br>
- **HELP** - Displays to the user a full list of commands along with their syntax <br>

### Student request resolver
The core functionality of EffyStudentDB is to resolve requests taken from the `STUDENTREQUEST` table. That table holds information on which students had the desire to join which course, based on their grades.
In the first iteration, if a given student have the minimal points requirement met, it will be automatically placed inside the `COURSEATTENDEES` table. Each course contain information about which students are currently attending it, and thus it will replace an entry with the lowest possible score with a new value whenver it is needed.
# Code & Framework
## Utilities
This section will primarily focus on how the application handles SQL communication, file and folder creation. The core principle of the utilities section is that it operates on a `Manager`-like structure. There are a few managers who have their separate responsibilities, which they delegate further into the `CommandHandler`.
-  `WorkspaceManager` - Handles the creation of the entire workspace structure. It delegates jobs to create folders, files, and fill up the schema and database files. <br>

**Workspace file creation**
```C++
bool WsManager::createFile(std::string name, std::optional<std::filesystem::path> subPath)
{
    LOG((*logger_), "fileName=", name, " subPath=", subPath.has_value() ? subPath.value() : "nullopt");
    if (fManager_->exist(name, subPath))
    {
        std::cout << "File: \"" << name << "\" exists at: " << workingDir_.string();
        if (subPath.has_value()) { std::cout << "/" << subPath.value().string(); }
        std::cout << "\n";
        std::cout << "Skipping...\n";
        return false;
    }
    std::cout << "Creating file: \"" << name << "\" at: " << workingDir_.string();
    if (subPath.has_value()) { std::cout << "/" << subPath.value().string(); }
    std::cout << "\n";

    bool rc = fManager_->createFile(name, subPath);
    return rc;
}
```
 **SQL Schema creation**
```C++
bool WsManager::createInitialSchema()
{
    using namespace Types;
    LOG((*logger_), "Creating the initial schema file with default tables");
    std::unique_ptr<std::fstream> schemaPtr = fManager_->getFile(fileBase, fileBaseSubdir);
    if (!schemaPtr->is_open())
    {
        LOG((*logger_), "Could not open file: ", fileDatabase, " with subdir: ", fileDatabaseSubdir);
        return false;
    }

    std::vector<Table> tables;
    tables.reserve(...);

    tables.push_back(defaultTable1());
    tables.push_back(defaultTable2());
    // ...
    tables.push_back(defaultTableN());

    *schemaPtr << "-- Effy.db - this file has been generated automatically\n";
    *schemaPtr << "-- Do not modify it!\n";
    for (const auto& tbl : tables)
    {
        LOG((*logger_), "Inserting ", tbl.getName(), " into database schema");
        *schemaPtr << tbl.makeFormula();
        sManager_->addTable(tbl);
    }

    LOG((*logger_), "Initial schema was written into ", "base.sql");
    schemaPtr->close();
    return true;
}
```
 **Directory verification**
```C++
bool WsManager::directoryExists(std::string directoryName, std::optional<std::filesystem::path> subPath)
{
    std::cout << "Looking for the directory: " << directoryName << "...\n";
    bool rc = dManager_->exist(directoryName, subPath);

    if (rc)
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
```
- `FileManager` and `DirectoryManager` - They both behave in a very similar pattern. They consist of main functionalities, application root directory information, and `CommandHandler`. Once a creation, deletion, or verification job is required from them, they will replace their current `CommandHandler` with one with a different type. Examples: <br>

**Directory creation**
  ```C++
  bool DirectoryManager::deleteDirectory(std::string directoryName, std::optional<std::string> subPath)
{
    setCommandHandler(std::make_unique<RemoveCommand>(directoryName, targetType::directory, subPath));
    if (!comHandler_->execute()) { return false; }
    return !exist(directoryName, subPath);
}
  ```
**File verification**
```C++
bool FileManager::exist(std::string fileName, std::optional<std::filesystem::path> subPath)
{
    setCommandHandler(std::make_unique<verifyCommand>(fileName, subPath));
    return comHandler_->execute();
}
```
- `CommandHandler` - An interface used to determine and execute which type of action must be executed based triggered from a specific manager. The actual job implementation is located here for both `DirectoryManager` and `FileManager`. Each command (`createCommand`, `removeCommand` and `verifyCommand`) overrides the `bool execute()` method to their needs.

**CreateCommand**
```C++
bool CreateCommand::execute()
{
    std::string fullPath = assambleFullPath(target_, subPath_, std::filesystem::current_path().string());

    switch (type_)
    {
        case targetType::directory :
            {
                std::filesystem::create_directory(fullPath);
                return true;
            }
        case targetType::file :
            {
                std::ofstream fileOut(fullPath);
                return true;
            }
        default : return false;
    }
}
```
**RemoveCommand**
```C++
bool RemoveCommand::execute()
{
    std::string fullPath = assambleFullPath(target_, subPath_, std::filesystem::current_path().string());

    switch (type_)
    {
        case targetType::directory : std::filesystem::remove_all(fullPath); return !std::filesystem::exists(fullPath);
        case targetType::file : std::filesystem::remove(fullPath); return !std::filesystem::exists(fullPath);
        default : return false;
    }
}
```
**VerifyCommand**
```C++
bool verifyCommand::execute()
{
    std::string fullPath = assambleFullPath(target_, subPath_, std::filesystem::current_path().string());
    return std::filesystem::exists(fullPath);
}
```
- `SqlManager` - Mainly used for communication between the application and the database. Its functionality will be tackled in the next chapter.
### SQL Communication
EffyStudentDB operates on a local `SQLite3` server file that is created every time the program cannot find it in the specified paths. The SQL communication is split into 2 sections:
#### SQL Adapter
The `SqlAdapter`'s primary purpose is to translate information obtained from the `SqlManager` into C++ objects, which will be used in core functionality processing, and it will also translate the data from these C++ objects into appropriate SQL queries. <br>
Database getters are separated into each concrete type:
```C++
std::vector<School> getSchools(std::string filter = "");
std::vector<Student> getStudents(std::string filter = "");
std::vector<Subject> getSubjects(std::string filter = "");
std::vector<Grade> getGrades(std::string filter = "");
std::vector<CourseSubjectWeight> getCourseSubjectWeight(std::string filter = "");
std::vector<Course> getCourses(std::string filter = "");
std::vector<Request::Srequest> getSrequests(std::string filter = "");
std::vector<std::shared_ptr<Entry>> getEntries(std::string tableName, std::string filter = "");
std::vector<rawAttendee> getAttendees();
```
However, setters database management options are mainly used on one abstract class which is `Entry`:
```C++
bool addEntry(Entry& newEntry);
bool updateEntry(const Entry& oldEntry, const Entry& newEntry);
bool removeEntry(const Entry& targetEntry, std::optional<std::string> condition = std::nullopt);
bool addGrade(Student& targetStudent, Subject& targetSubject, const float& grade);
bool removeGrade(const Student& targetStudent, const Subject& targetSubject);
```
Each of the database modifying options are usually operating on the similar idea: 1) Create filter 2) Execute aproporiate command:
```C++
bool SqlAdapter::removeAttendee(const uint16_t& studentId, const uint16_t& courseId)
{
    LOG((*logger_), "Removing attendee ", studentId, " from course ", courseId);
    Table targetTable = sManager_->getTableSchema(g_tableCourseAttendees);
    std::unordered_map<std::string, std::string> attrs;
    attrs.insert(std::make_pair("studentId", std::to_string(studentId)));
    attrs.insert(std::make_pair("courseId", std::to_string(courseId)));
    std::string filter = makeFilter(attrs);
    if (sManager_->removeEntryFromTable(targetTable.getName(), filter))
    {
        LOG((*logger_), "Successfully deleted attendee");
        return true;
    }
    LOG((*logger_), "Could not delete attendee");
    return false;
}
```
#### SQL Manager
This is a core framework module that handles not only database connection but also database creation.<br>
**Database creation**
`Table` and its `Attributes` (or columns) are defined as a separate classes. Each `Attribute` contains information about its name, type, traits (like `UNIQUE` or `NOT NULL`), and references (Foreign keys). Each `Table` contains information about its name, schema (map of attributes), and foreign keys to different tables. <br>
`Table` class enables the user to flexible modify, add or remove tables from the overall schema. The `makeFormula` will automatically create a `CREATE TABLE` SQL query with all of its attributes properly formatted (reflected in `schema_`). Currently, each working table has its own `default<name>Table()` function that assembles a predefined SQL Table schema. <br>
Example default definition:
```C++
Types::Table defaultStudentsTable()
{
    Types::Table studentTbl(g_tableStudents);
    studentTbl.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
    studentTbl.addToSchema({ "firstName", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL } });
    studentTbl.addToSchema({ "secondName", Types::AttributeType::SQL_TEXT, {} });
    studentTbl.addToSchema({ "lastName", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL } });
    studentTbl.addToSchema({
        "email", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE }
    });
    studentTbl.addToSchema({ "schoolId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
    studentTbl.linkAttributes(studentTbl.getAttributeByName("schoolId"), "Schools", "id");
    return studentTbl;
}
```
To add or remove `Table` from the creation list, simply change the `createInitialSchema` function inside the `WorkspaceManager`.
## Table overview
There is a total of 8 tables inside the current stage of EffyStudentDb development. Most of them are self-explanatory, such as `subjects` which only consists of ID and name. However, there are tables that are primarily used to connect 2 different tables and these will be presented here:
- Grades

|   Grade ID  |  Student ID |  Subject ID | Grade value  |
| ----------- | ----------- | ----------- | ------------ |
| Primary key | Foreign key | Foreing key | Float / REAL |

Used to monitor which student got what grade from which subject.

- CourseSubjectWeight

|   Weight ID  |  Course ID  |  Subject ID | weight value |
| ------------ | ----------- | ----------- | ------------ |
| Primary key  | Foreign key | Foreing key | Float / REAL |

Used to monitor what is the multiplication value of each subject for a given course.

- CourseAttendees

|   Attendee ID  |  Course ID  |  Student ID | points value |
| -------------- | ----------- | ----------- | ------------ |
| Primary key    | Foreign key | Foreing key | Float / REAL |

Used to monitor which student is currently enrolled in which course and how many points does that student have. The `DEFAULT` value for `points values` is 100.

- StudentRequest

|   Request ID   |  Course ID  |  Student ID |    Status    |
| -------------  | ----------- | ----------- | ------------ |
| Primary key    | Foreign key | Foreing key | Integer      |

Used to monitor which student desired to join which course, with a status of that request. Statuses are: `Approved` = 0, `Denied` = 1, `Pending` = 2, `Unknown` = 3.

## Core functionality
This section will describe how each of the two core functionalities works. They both operate on `Core::Types` that are the objective representation of the SQL database. Each type is derived from a parent class `Core::Types::Entry` and overrides its common functionalities. If a new table is presented with a clear object structure, it has to be implemented as an additional `Entry` derived. <br>
Entry structure:
```C++
namespace Core::Types
{

    struct Entry
    {
        uint16_t id_;
        const std::string associatedTable_;

        Entry(const Entry&) = default;

        Entry(std::string table):
            id_(0),
            associatedTable_(table)
        {}

        Entry(uint16_t id, std::string table):
            id_(id),
            associatedTable_(table)
        {}

        virtual ~Entry() {};
        virtual std::string toString() const = 0;
        virtual std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) = 0;
        inline virtual std::map<std::string, std::string> getAttrs() const
        {
            return {
                { "id", std::to_string(id_) }
            };
        };
        virtual std::shared_ptr<Entry> mirrorMissing(const std::shared_ptr<Entry> other) = 0;
        virtual Entry& operator= (const Entry& other) = 0;
    };

}  // namespace Core::Types
```
- `std::string toString()` - returns a string of all of the objects' values.
- `std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true)` - Prompts the user to create a copy of the given concrete `Entry`; if `makeFull` is set to `false`, it will allow the user to leave column values empty.
- `std::map<std::string, std::string> getAttrs()` - returns a map of all of the attributes with their values as strings. These have to be manually defined. The name of the variable is used as a key, where its value is used as a value.<br>
**NOTE**: Key must have the same name as the respective SQL column name.
- `std::shared_ptr<Entry> mirrorMissing(const std::shared_ptr<Entry> other)` - returns a smart (shared) pointer to a new object that has its missing values filled from the `other` parameter.
- `Entry& operator= (const Entry& other)` - assign every value from an `other` concrete `Entry` to the current one.
### Management
Every `Entry` is being stored inside the `SessionData` class, which only operates on the abstract interface. While being initialized, it populates its `entryList_` with all of the table names as keys. The value for each key is another map. `std::map<uint16_t, std::shared_ptr<Entry>>` aliased as `abstractTypeList`. It loads all the data from the database once. This means that each modification in the database made during the session must be reevaluated and repopulated. This is done inside the `Session` class. `Session` class also provides all of the data from the `SqlAdapter` to the `SessionData`, during the `fetchX` methods. The `Session` delegates the output stream job to the `menu` class and the input stream to the `InputHandler` functions. Once the user enters the `Manage database` option from the main menu, `session` will also handle every action that the user provides.
#### Commands
Every time a user enters an action, it will be turned into a `Command` object that is created via the `CommandFactory` based on the tokens provided by the user. There are several implicit types of commands, and each of them has a different type of handling:
- `Simple`: These commands are single-token that the user had provided. These commands are mainly limited to simple meta-management actions. The commands are: `exit`, `help`, `list`
- `Basic table-based`: These commands are made of 2 tokens, and state action type and action target. Some of the tokens are: `add`, `remove`, or `edit`.
- `Course management commands`: Their purpose is solely on managing courses - opening them and closing them for the recuritment process. (`open` & `close`)
- `Link-based`: They are used to assign or drop student from a given course. They consist of 3 tokens and are the longest possible commands. (`assign` & `drop`)

Each command is a derrived of the `ICommand`, and are forced to define their `exec` and `name` methods. Each new command must derive from this class interface.
```C++
class ICommand
{
  protected:
    std::shared_ptr<Utilities::Logger> logger_;

  public:
    ICommand(std::shared_ptr<Utilities::Logger> logger):
        logger_(logger) {};
    virtual bool exec()              = 0;
    virtual ~ICommand()              = default;
    virtual std::string name() const = 0;
};
```

Commands are validated and constructed in the body of `makeCommand` in `CommandFactory`, this validation and assembly must be adjusted whenever a new command is introduced into the database tool. 
### Request resolver
Each students that wish to join a given course must create a unique StudentRequest, these will be used in this class. 
`RequestResolver` fetches every student, course (along with its attendees and weights) and grade, and iterate it one by one, calculating the amount of points a given student will get if all of their present grades were summed up. The formula for score calculation looks as follows:<br>
`[(1st subject grade * weight) + (2nd subject grade * weight) + ... + (N-th subject grade * weight)] / N` <br>
If students `points >= Course::baseMinimalPoints_` then they will be added into a pending attendees list; however, if their position inside the `attendees_` = `min_` (`min_` meaning that their score is the lowest possible score inside the attendees list), they will be removed in the moment that another request will surpas the `baseMinimalPoints_`. 
Each student present in `acceptedStudents_` pending list will be later on added to the database. If a student is not present inside such list, that means that they did not passed into any of the courses they wished for and thus their request will automatically be marked as denied.
# Generator
It's a Python module that is primarily used to generate testing entries for the `SQL` server. <br>
The generator will handle every table, creating a predefined number of entries for each. The flow of the generator is simple, it will first handle simple table types, such as subjects or schools. It will load names from the constant list of names, insert them and finally assign them ID, returning to the main flow. Next the complex types are handled which require a little bit more work. First courses are handled, added, and an ID assigned, then the subject mapping is inserted into the database. Lastly, students are added. Names, emails, grades, and student requests are randomly generated, as well as which school a student attended. Each generation run will produce a set of entries and delete existing ones so that the database stays healthy with its data integrity. <br>
There are 2 ways of triggering a generation.
1. Usage of the `run_generator.sh` while being inside the `generator/` folder. This script will trigger the `generate.py` and insert the output to a file `generator_output.txt`.
2. Manually run the `generate.py`. This program does not require any argument and will immidietly start working. Just as the first method it is required to run this script inside the `generator/` path.
