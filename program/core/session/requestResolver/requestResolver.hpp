#pragma once

#include "../../../types/course/course.hpp"
#include "../../../types/courseSubjectWeight/courseSubjectWeight.hpp"
#include "../../../types/grade/grade.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/studentRequest/request.hpp"
#include "../../../utilities/logger/logger.hpp"
#include "../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../data/data.hpp"

#include <functional>
#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

namespace Core
{
    using Utilities::Logger;
    using Utilities::Sql::SqlAdapter;

    using namespace Core::Types;

    class RequestResolver
    {
      private:
        std::shared_ptr<SessionData> sesData_;
        std::shared_ptr<Logger> logger_;
        std::shared_ptr<SqlAdapter> sqlAdapter_;
        std::vector<Types::Request::Srequest> pendingRequests_;

        std::unordered_set<uint16_t> extractIds(std::function<uint16_t(Request::Srequest)> extractConcrete) const;
        template<typename T>
        std::map<uint16_t, T> loadEntriesFromIds(
            std::unordered_set<uint16_t> ids, std::function<std::vector<T>(std::string)> sqlGetter) const;
        template<typename Origin, typename Info>
        void loadAdditionalInfo(std::map<uint16_t, Origin>& orderedEntries,
            std::function<std::vector<Info>(std::string)> sqlGetter,
            std::map<uint16_t, std::shared_ptr<Info>> Origin::* infoHolder) const;
        void assignAttendees(std::map<uint16_t, Course>& courses, const std::map<uint16_t, Student>& students);
        float calculatePoints(const Student& invoker, Course& target);

      public:
        RequestResolver(
            std::shared_ptr<Logger> logger, std::shared_ptr<SqlAdapter> sqlAdapter, std::shared_ptr<SessionData> sesData);
        ~RequestResolver() = default;
        void run();
    };

    template<typename T>
    std::map<uint16_t, T> RequestResolver::loadEntriesFromIds(
        std::unordered_set<uint16_t> ids, std::function<std::vector<T>(std::string)> sqlGetter) const
    {
        LOG((*logger_), "Extracting concrete entries from IDs (n=", ids.size(), ")");
        std::stringstream filter;
        filter << "id IN (";
        for (const auto& id : ids) { filter << id << ","; }
        filter.seekp(-1, filter.cur);
        filter << ")";
        std::map<uint16_t, T> result;
        std::vector<T> unorderedEntries = sqlGetter(filter.str());
        for (const auto& entry : unorderedEntries) { result.insert(std::make_pair(entry.id_, entry)); }
        LOG((*logger_), "Extracted entires successfully");
        return result;
    }

    template<typename Origin, typename Info>
    void RequestResolver::loadAdditionalInfo(std::map<uint16_t, Origin>& orderedEntries,
        std::function<std::vector<Info>(std::string)> sqlGetter,
        std::map<uint16_t, std::shared_ptr<Info>> Origin::* infoHolder) const
    {
        LOG((*logger_), "Getting additional info for entries. n=", orderedEntries.size());
        for (auto& [entryId, entry] : orderedEntries)
        {
            // We're operating on 4 different tables, and our targets have 2 different sets of IDs.
            // Its required to determine what ID is desired by us - its done by simple check of associated table.
            std::stringstream filter;
            filter << (entry.associatedTable_ == g_tableCourses ? "CourseId = " : "StudentId = ")
                   << entryId;  // Its always either student or course
            std::vector<Info> additionalInfo = sqlGetter(filter.str());
            std::for_each(additionalInfo.begin(), additionalInfo.end(), [infoHolder, &entry](Info val) {
                (entry.*infoHolder).insert(std::make_pair(val.id_, std::make_shared<Info>(val)));
            });
        }
        LOG((*logger_), "Finalized getting additional information.");
    }
}  // namespace Core
