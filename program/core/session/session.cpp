#include "session.hpp"

#include "../../types/entry.hpp"
#include "../../utilities/common/constants.hpp"
#include "../../utilities/common/stringManip.hpp"

using namespace Utilities::Common::Constants;

Session::Session(std::shared_ptr<WsManager> wsMgr):
	wsMgr_(wsMgr),
	logger_(wsMgr_->getLogger()),
	sAdapter_(std::make_unique<SqlAdapter>(logger_, wsMgr_->getSqlManager())),
	sesData_(std::make_shared<SessionData>()),
	display_(std::make_unique<Menu>(logger_, sesData_))
{
	LOG((*logger_), "Loading initial database entries");
	fetchAll();
	LOG((*logger_), "Session established");
}

void Session::fetchAll()
{
	LOG((*logger_), "Fetching all entries from the database");
	fetchSchools();
	fetchStudents();
	fetchSubjects();
	fetchGrades();
	fetchCourses();
	fetchSrequests();
	LOG((*logger_), "Fetching done");
}

void Session::fetchSchools()
{
	LOG((*logger_), "Fetching schools");
	std::vector<Core::Types::School> dbSchools = sAdapter_->getSchools();
	LOG((*logger_), "Got ", dbSchools.size(), " Entries");
	for (const auto& entry : dbSchools) { sesData_->addEntry(std::make_shared<School>(entry)); }
}

void Session::fetchStudents()
{
	LOG((*logger_), "Fetching students");
	std::vector<Core::Types::Student> dbStudents = sAdapter_->getStudents();
	LOG((*logger_), "Got ", dbStudents.size(), " Entries");
	for (const auto& entry : dbStudents)
	{
		std::shared_ptr<Student> concreteStudent = std::make_shared<Student>(entry);
		sesData_->addEntry(concreteStudent);

		const uint16_t schoolId              = entry.schoolId_;
		std::shared_ptr<School> targetSchool = std::dynamic_pointer_cast<School>(sesData_->getEntry(schoolId, g_tableSchools));
		if (targetSchool) { (*targetSchool).students_.insert(std::make_pair(entry.id_, concreteStudent)); }
		else { std::runtime_error("Could not fetch school data with the following data: ID=" + std::to_string(schoolId)); }
	}
}

void Session::fetchSubjects()
{
	LOG((*logger_), "Fetching subjects");
	std::vector<Core::Types::Subject> dbSubjects = sAdapter_->getSubjects();
	LOG((*logger_), "Got ", dbSubjects.size(), " Entries");
	for (const auto& entry : dbSubjects) { sesData_->addEntry(std::make_shared<Subject>(entry)); }
}

void Session::fetchCourses()
{
	LOG((*logger_), "Fetching courses");
	std::vector<Core::Types::Course> dbCourses = sAdapter_->getCourses();
	for (const auto& entry : dbCourses) { sesData_->addEntry(std::make_shared<Course>(entry)); }
}

void Session::fetchSrequests()
{
	LOG((*logger_), "Fetching student requests");
	std::vector<Core::Types::Request::Srequest> dbSrequests = sAdapter_->getSrequests();
	for (const auto& entry : dbSrequests) { sesData_->addEntry(std::make_shared<Srequest>(entry)); }
}

void Session::run()
{
	LOG((*logger_), "Main run function called");
	bool exit = false;
	do {
		Core::Display::MainMenuOption op = display_->showMainMenu();
		switch (op)
		{
			case Core::Display::MainMenuOption::manageDb :
				{
					Action command;
					do {
						command = display_->manageDatabase();
						handleAction(command);
					} while (command.getCommand() != "EXIT");

					break;
				}
			case Core::Display::MainMenuOption::handleRqs : break;
			case Core::Display::MainMenuOption::exit : exit = true; LOG((*logger_), "Exiting application");
			default : break;
		}

	} while (!exit);
}

bool Session::handleAction(const Action& userAction)
{
	LOG((*logger_), "Executing action");
	// Action is correct here - guaranteed
	std::string userCommand = userAction.getCommand();

	LOG((*logger_), "Action got command: ", userCommand);
	if (userCommand == "EXIT")
	{
		LOG((*logger_), "Normal exit");
		std::cout << "Exiting management mode.\n";
		return true;
	}

	std::string userTarget = userAction.getTarget();
	LOG((*logger_), "Action got target: ", userTarget);

	std::shared_ptr<Entry> concreteEntry = makeConcreteType(userTarget);

	// Select correct function
	// This is WIP
	if (userCommand == "ADD")
	{
		LOG((*logger_), "Abstract add: ", userAction.getTarget());
		std::cout << "New entry:\n";
		concreteEntry.get()->userConstruct();
		if (sAdapter_->addEntry(*concreteEntry))
		{
			sesData_->addEntry(concreteEntry);
			onAdd(concreteEntry);
			return true;
		}
	}

	// Now TMP will act as filter holder
	std::cout << "Matching values:\n";
	std::unordered_map<std::string, std::string> attrs = concreteEntry->userConstruct(false);
	std::string filter                                 = sAdapter_->makeFilter(attrs, Utilities::Sql::filterAnyMatch);
	// @TODO - if filter is empty, delete all? or do nothing?

	LOG((*logger_), "Filter to lookup: ", filter);
	std::vector<std::shared_ptr<Entry>> affectedEntries = sAdapter_->getEntries(userTarget, filter);

	if (affectedEntries.empty())
	{
		std::cout << "No match was found in the database!\n";
		return true;
	}

	if (userCommand == "REMOVE" || userCommand == "UPDATE") { std::cout << "Affected entries:\n"; }

	for (const auto& entry : affectedEntries) { std::cout << entry->toString() << "\n"; }

	// Nothing much has to be done while finding
	if (userCommand == "FIND") { return true; }

	// For each affected entry delete
	if (userCommand == "REMOVE")
	{
		for (const auto& val : affectedEntries)
		{
			if (sAdapter_->removeEntry(*val))
			{
				onDelete(val);
				sesData_->removeEntry(val->id_, userTarget);
			}
		}
		return true;
	}

	// For each affected entry update
	if (userCommand == "UPDATE")
	{
		// Get new values 1st
		std::cout << "Insert new values:\n";
		std::shared_ptr<Entry> updatedValues = makeConcreteType(userTarget);
		std::cout << "DBG!: Always leave update\n";
		return true;

		// Update each affected entry
		for (const auto& val : affectedEntries) { sesData_->updateEntry(val->id_, updatedValues); }
	}

	//@TODO verify linking

	return false;
}

void Session::onAdd(const std::shared_ptr<Entry> newEntry)
{
	const std::string targetTable = newEntry->associatedTable_;
	LOG((*logger_), "Called onAdd linking cleanup - target table: ", targetTable);
	// School, Subject, StudentRequest and Course does not need a special handle
	if (targetTable == g_tableCourses || targetTable == g_tableSubjects || targetTable == g_tableSchools ||
		targetTable == g_tableStudentRequest)
	{
		LOG((*logger_), "Simple type - no clean needed");
		return;
	}

	// If a new student is present add it to the school list and sRequests
	if (targetTable == g_tableStudents)
	{
		std::shared_ptr<Student> concreteStudent = std::dynamic_pointer_cast<Student>(newEntry);
		LOG((*logger_), "Student linking - id: ", concreteStudent->id_, " schoolId: ", concreteStudent->schoolId_);

		// Only add to the target school
		std::shared_ptr<School> studentSchool =
			std::dynamic_pointer_cast<School>(sesData_->getEntry(concreteStudent->schoolId_, g_tableSchools));
		if (!studentSchool)
		{
			LOG((*logger_), "Student added to a non existing school! This may lead to undefined behavior. Student signature: ",
				concreteStudent, " id: ", concreteStudent->id_);
			return;
		}
		std::dynamic_pointer_cast<School>(sesData_->getEntry(concreteStudent->schoolId_, g_tableSchools))
			->students_.insert(std::make_pair(newEntry->id_, concreteStudent));
	}

	// No grades and course subject weight impl yet so we stick to the student just for now.
}

void Session::onDelete(const std::shared_ptr<Entry> targetEntry)
{
	const uint16_t id             = targetEntry->id_;
	const std::string targetTable = targetEntry->associatedTable_;
	LOG((*logger_), "Called onDelete linking cleanup - target table: ", targetTable);

	// There are little to no safe types and each have to be handled.
	// If a school is deleted - inform about leaving students without school
	if (targetTable == g_tableSchools)
	{
		std::shared_ptr<School> concreteSchool = std::dynamic_pointer_cast<School>(targetEntry);
		LOG((*logger_), "Deleting school id: ", id, " name: ", concreteSchool->name_);
		if (!concreteSchool->students_.empty())
		{
			std::cout << "Deleting " << concreteSchool << " left " << concreteSchool->students_.size() << " without any school\n";
		}
		return;
	}

	// If a student is deleted - grades are erased automatically, schools has to remove one entry from list and associated student
	// request deleted.
	if (targetTable == g_tableStudents)
	{
		std::shared_ptr<Student> concreteStudent = std::dynamic_pointer_cast<Student>(targetEntry);
		LOG((*logger_), "Deleting student id: ", id);
		// Remove student from a given school
		LOG((*logger_), "Student linking - id: ", id, " schoolId: ", concreteStudent->schoolId_);
		std::shared_ptr<School> studentSchool =
			std::dynamic_pointer_cast<School>(sesData_->getEntry(concreteStudent->schoolId_, g_tableSchools));
		if (studentSchool)
		{
			LOG((*logger_), "Removing student id = ", id, " from assoc school: ", concreteStudent->schoolId_);
			studentSchool->students_.erase(id);
		}

		// Remove students' requests
		std::unique_ptr<abstractTypeList> requestLists = sesData_->getEntries(g_tableStudentRequest);
		if (!requestLists)
		{
			LOG((*logger_), "No student reuqests table found to delete given student");
			return;
		}

		for (const auto& request : *requestLists)
		{
			std::shared_ptr<Srequest> req = std::dynamic_pointer_cast<Srequest>(request.second);
			if (req->studentId_ == id) { requestLists->erase(req->id_); }
		}

		LOG((*logger_), "Linking cleaned with deleted student\n");
		return;
	}

	// If a given subject is deleted we only need to handle the course to subject weight - and eventually student grade with given
	// id

	// If a given course is deleted a student request with that course id will be declined, the subject weight will be erased
	// automatically

	// If a given grade is deleted only delete it from target student - no handle yet

	// If a course weight is deleted only delete it from target course - no handle yet

	// @TODO handle rest
}

void Session::onUpdate(const std::shared_ptr<Entry> oldEntry, const std::shared_ptr<Entry> newEntry)
{
	const uint16_t id             = oldEntry->id_;
	const std::string targetTable = oldEntry->associatedTable_;
	LOG((*logger_), "Editing entry id:", id, " targetTable:", targetTable);
}

std::shared_ptr<Entry> Session::makeConcreteType(const std::string& tableName) const
{
	LOG((*logger_), "Getting concrete type from target table: ", tableName);
	std::shared_ptr<Entry> tmp;
	// Extract concrete type
	if (tableName == g_tableSchools) { tmp = std::make_shared<School>(); }
	else if (tableName == g_tableStudents) { tmp = std::make_shared<Student>(); }
	else if (tableName == g_tableSubjects) { tmp = std::make_shared<Subject>(); }
	else if (tableName == g_tableCourses) { tmp = std::make_shared<Course>(); }
	return tmp;
}

bool Session::updateSchool(School& targetSchool, School& newSchool)
{
	LOG((*logger_), "Editing existing school");
	if (sAdapter_->updateEntry(targetSchool, newSchool))
	{
		sesData_->updateEntry(targetSchool.id_, std::make_shared<School>(newSchool));
		return true;
	}
	return false;
}

bool Session::updateCourse(Course& targetCourse, Course& newCourse)
{
	LOG((*logger_), "Updating existing course.");
	if (sAdapter_->updateEntry(targetCourse, newCourse))
	{
		sesData_->updateEntry(targetCourse.id_, std::make_shared<Course>(newCourse));
		return true;
	}
	return false;
}

void Session::fetchGrades()
{
	LOG((*logger_), "Fetching grades");
	std::vector<std::vector<std::string>> dbGrades = sAdapter_->getGrades();
	LOG((*logger_), "Got ", dbGrades.size(), " Entries");
	if (!dbGrades.empty())
	{
		LOG((*logger_), "Linking grades")
		for (auto& entry : dbGrades)
		{
			// (0) GRADE | (1) SUBID | (2) STUDENTID | (3) ID
			float currentGrade       = std::stof(entry.at(0));
			uint16_t targetedStudent = std::stoul(entry.at(1));
			uint16_t targetedSubject = std::stoul(entry.at(2));
			sesData_->addGrade(targetedSubject, targetedStudent, currentGrade);
		}
	}
}

Session::~Session() { LOG((*logger_), "Session teardown!"); }