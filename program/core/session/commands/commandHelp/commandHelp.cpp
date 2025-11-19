#include "commandHelp.hpp"

namespace Core::Commands
{
    bool CommandHelp::exec()
    {
        LOG((*logger_), "Showing help");
        std::stringstream helpMsg;
        helpMsg << "Management mode allows the user to alter entries inside the database with given commands.\n";
        helpMsg << "Commend & their usage:\n";
        helpMsg << "ADD <TargetTable> - Will launch prompt to insert new entry into given table.\n";
        helpMsg
            << "REMOVE <TargetTable> <TargetId - opt> - Will launch a prompt and delete every entry matching given patterns.\n";
        helpMsg << "If an ID is provided it will delete only that one entry.\n";
        helpMsg << "EDIT <TargetTable> <TargetId - opt> - Will launch a prompt and alter every entry matching given patterns "
                   "with new "
                   "values.\n";
        helpMsg << "ASSIGN <TargetCourseId> <TargetStudentId> - Assigns a student to a given course\n";
        helpMsg << "DROP <TargetCourseId> <TargetStudentId> - Drop a student from a given course\n";
        helpMsg << "CLOSE <TargetCourseId> - Closes recruitment for a given course\n";
        helpMsg << "OPEN <TargetCourseId> - Opens recruitment for a given course\n";
        helpMsg << "If an ID is provided it will only update that one entry.\n";
        helpMsg << "FIND <TargetTable> <TargetId - opt> - WIll launch a prompt and display every entry matching given pattern.\n";
        helpMsg << "If an ID is provided it will only display that one entry\n";
        helpMsg << "LIST - Will provide the user with the full list of table names.\n";
        helpMsg << "SHOW <TargetTable> - Will show entries from a given table.\n";
        helpMsg << "HELP - Displays this message.\n";
        std::cout << helpMsg.str();
        return true;
    }
}  // namespace Core::Commands

