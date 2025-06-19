#include "data.hpp"

#include "../../../utilities/common/constants.hpp"

#include <iostream>

using namespace Utilities::Common::Constants;

SessionData::SessionData()
{
	// Initialize the entry list with empty maps for each table
	entryList_.insert(std::make_pair(g_tableSchools, std::map<uint16_t, std::shared_ptr<Entry>>()));
	entryList_.insert(std::make_pair(g_tableStudents, std::map<uint16_t, std::shared_ptr<Entry>>()));
	entryList_.insert(std::make_pair(g_tableSubjects, std::map<uint16_t, std::shared_ptr<Entry>>()));
	entryList_.insert(std::make_pair(g_tableCourses, std::map<uint16_t, std::shared_ptr<Entry>>()));
	entryList_.insert(std::make_pair(g_tableGrades, std::map<uint16_t, std::shared_ptr<Entry>>()));
	entryList_.insert(std::make_pair(g_tableStudentRequest, std::map<uint16_t, std::shared_ptr<Entry>>()));
	entryList_.insert(std::make_pair(g_tableCourseSubjectWeight, std::map<uint16_t, std::shared_ptr<Entry>>()));
}

std::set<std::string> SessionData::getTableNames() const
{
	std::set<std::string> names;
	for (const auto& table : entryList_) { names.insert(table.first); }
	return names;
}

bool SessionData::verifyTable(std::string tableName) const
{
	if (!entryList_.contains(tableName))
	{
		std::cout << "No table named: " << tableName << "\n";
		return false;
	}
	return true;
}

void SessionData::addEntry(const std::shared_ptr<Entry> newEntry)
{
	if (!verifyTable(newEntry->associatedTable_)) { return; }

	std::map<uint16_t, std::shared_ptr<Entry>>& concreteMap = entryList_.at(newEntry->associatedTable_);
	if (!concreteMap.contains(newEntry->id_)) { concreteMap.insert(std::make_pair(newEntry->id_, newEntry)); }
}

void SessionData::removeEntry(const uint16_t targetId, const std::string& associatedTable)
{
	if (!verifyTable(associatedTable)) { return; }
	std::map<uint16_t, std::shared_ptr<Entry>>& concreteMap = entryList_.at(associatedTable);
	if (concreteMap.contains(targetId)) { concreteMap.erase(targetId); }
}

void SessionData::updateEntry(const uint16_t targetId, const std::shared_ptr<Entry> alteredEntry)
{
	if (!verifyTable(alteredEntry->associatedTable_))
	{
		return;
	}

	std::cout << "DBG: Target altered entry: " << alteredEntry.get()->toString() << "\n";
	abstractTypeList& concreteMap = entryList_.at(alteredEntry->associatedTable_);
	if (concreteMap.contains(targetId))
	{
		std::cout << "DBG: Before altering entry: " << concreteMap.at(targetId).get()->toString() << "\n";
		*concreteMap.at(targetId) = *alteredEntry->fillGaps(concreteMap.at(targetId));
		std::cout << "DBG: Altered entry: " << concreteMap.at(targetId).get()->toString() << "\n";
	}
	else { std::cout << "!!! No such entry with id " << targetId << " in table " << alteredEntry->associatedTable_ << "\n"; }
}

bool SessionData::isPresent(const uint16_t targetId, const std::string& associatedTable) const
{
	if (!verifyTable(associatedTable)) { return false; }
	return entryList_.at(associatedTable).contains(targetId);
}

std::shared_ptr<Entry> SessionData::getEntry(const uint16_t targetId, const std::string& associatedTable)
{
	if (!verifyTable(associatedTable)) { return nullptr; }

	if (entryList_.at(associatedTable).contains(targetId)) { return entryList_.at(associatedTable).at(targetId); }

	return nullptr;
}

std::shared_ptr<abstractTypeList> SessionData::getEntries(const std::string& table)
{
	if (!verifyTable(table))
	{
		std::cout << "No such table: " << table << "\n";
		return nullptr;
	}

	return std::make_shared<abstractTypeList>(entryList_.at(table));
}
