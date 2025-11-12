#include "menu.hpp"

#include "../../utilities/common/constants.hpp"
#include "../../utilities/common/stringManip.hpp"
#include "../../utilities/inputHandler/inputHandler.hpp"

namespace Core::Display
{
    Menu::Menu(std::shared_ptr<Utilities::Logger> logger, std::shared_ptr<SessionData> sesData, std::shared_ptr<SqlAdapter> sqlAdapter) : logger_(logger),
                                                                                                                                          sesData_(sesData),
                                                                                                                                          inHandler_(Utilities::InputHandler()),
                                                                                                                                          dsplHelper_(Core::Display::Helper(logger, sqlAdapter, sesData))
    {
        LOG((*logger_), "Menu object created");
    }

    Action Menu::manageDatabase()
    {
        using namespace ActionType::Short;
        LOG((*logger_), "Entering Database management mode");
        Action userAction;
        std::string cmd;
        bool isValidAction;
        do {
            userAction    = createAction();
            cmd           = userAction.getCommand();
            isValidAction = validateAction(userAction);
            // Handle help and entry display here
            LOG((*logger_), "Action got command: ", cmd);
            if (isValidAction)
            {
                if (cmd == actionShow) { showEntries(userAction.getTarget()); }
                else if (cmd == actionHelp) { showHelp(); }
                else if (cmd == actionList) { listTables(); }
                LOG((*logger_), "Valid Action command: ", cmd, " target: ", userAction.getTarget());
            }
            else
            {
                LOG((*logger_), "Got invalid action. Cmd = ", cmd, " target = ", userAction.getTarget());
                std::cout << "Invalid action! Use \"" << actionHelp << "\" for available actions.\n";
            }
        } while ((cmd == actionShow || cmd == actionHelp || cmd == actionList) || !isValidAction);
        return userAction;
    }

    void Menu::showHelp() const
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
        return;
    }

    void Menu::showSelection(const std::vector<std::shared_ptr<Entry>>& selection) const
    {
        for (const auto& e : selection) { std::cout << e->toString() << "\n"; }
    }

    void Menu::listTables() const
    {
        LOG((*logger_), "Printing current tables");
        std::cout << "Current tables:\n";
        for (const auto& tables : sesData_->getTableNames()) { std::cout << tables << "\n"; }
    }

    Action Menu::createAction()
    {
        LOG((*logger_), "Creating action");
        std::string rawInput = inHandler_.getStringBeauty("Instruction");
        LOG((*logger_), "Received input to assemble action: \"", rawInput, "\"");
        handleIndirectAction(rawInput);
        Action userAction = Action(inHandler_.toUpper(rawInput));
        return userAction;
    }

    void Menu::handleIndirectAction(std::string& command)
    {
        using Utilities::Common::Constants::g_tableCourseAttendees;
        using Utilities::Common::Constants::g_tableCourses;
        LOG((*logger_), "Checking for indirect action in ", command);
        std::vector<std::string> tokens = Utilities::Common::tokenize(command, ' ');
        if (tokens.empty())
        {
            LOG((*logger_), "Command empty, skipping for validation check...");
            return;
        }
        std::string commandToken = inHandler_.toUpper(tokens.at(0));
        if (!Action::isCommandIndirect(commandToken))
        {
            LOG((*logger_), "Command is not indirect action skipping...");
            return;
        }
        // To properly handle request - table name is required at 2nd position
        std::string targetTable = "";
        if (commandToken == ActionType::Indirect::actionDrop || commandToken == ActionType::Indirect::actionAssign)
        {
            targetTable = g_tableCourseAttendees;
        }
        else { targetTable = g_tableCourses; }
        tokens.insert(tokens.begin() + 1, g_tableCourseAttendees);
        command = Utilities::Common::assemble(tokens, ' ');
        LOG((*logger_), "Properly handled indirect action. New action =: \"", command, "\"");
    }

    void Menu::showEntries(const std::string& targetTable) const
    {
        LOG((*logger_), "Showing entries from table: ", targetTable);
        const std::shared_ptr<abstractTypeList> concreteList = sesData_->getEntries(targetTable);

        if (!concreteList)
        {
            LOG((*logger_), "Could not find table: ", targetTable);
            return;
        }

        std::cout << "Displaying " << targetTable << ": \n";
        std::cout << concreteList->size() << " entries\n";
        for (const auto& [entryId, entryObj] : *concreteList)
        {
            std::cout << entryObj->toString() << "\n";
            if(targetTable == g_tableCourses)
            {
                dsplHelper_.displayAttendees(entryId);
            }
            else if(targetTable == g_tableStudents)
            {
                dsplHelper_.displayAttendedCourses(entryId);
                dsplHelper_.displayGrades(entryId);
            }
            std::cout << std::endl;
        }
    }

    bool Menu::validateAction(const Action& act)
    {
        const std::string& command = act.getCommand();

        if (command.empty())
        {
            LOG((*logger_), "Action command empty - aborting");
            return false;
        }

        if (!Action::isCommandValid(command)) { return false; }

        // First check the shortest commands
        if (Action::isCommandShort(command)) { return true; }

        const std::string& target = act.getTarget();
        // Look for 2nd arg
        if (target.empty())
        {
            LOG((*logger_), "Action target empty while command == ", command);
            return false;
        }

        // 2nd Arg is always a table name for these commands
        if (Action::isCommandDirect(command))
        {
            if (!sesData_->getTableNames().contains(target))
            {
                LOG((*logger_), "Attempted to operate on non-existent target table: ", target);
                return false;
            }
        }
        else
        {
            if (act.getAdditionalValues().empty())
            {
                LOG((*logger_), "Action additional values empty while command == ", command);
                return false;
            }
        }

        // @TODO add optional handle
        return true;
    }

    bool Menu::promptAlterAll(std::string filter, uint16_t count) const
    {
        LOG((*logger_), "Delete all detected - double check prompt");
        std::cout << "You are about to delete every entry (" << count << ") matching filter: \"" << filter << "\".\n";
        std::cout << "Are you sure? - this cannot be undone.\n";
        if (inHandler_.getYesOrNo() == 'Y')
        {
            LOG((*logger_), "User agreed");
            std::cout << "Agreed\n";
            return true;
        }
        LOG((*logger_), "User aborted")
        std::cout << "Abort\n";
        return false;
    }

    MainMenuOption Menu::showMainMenu()
    {
        MainMenuOption op;
        LOG((*logger_), "Showing main menu");
        std::cout << "-=# Main Menu #=-\n";
        std::cout << "1. Manage Database\n";
        std::cout << "2. Handle student requests\n";
        std::cout << "3. Exit.\n";
        op = static_cast<MainMenuOption>(inHandler_.getOption(1, 3));
        LOG((*logger_), "Got option ", op);
        return op;
    }
}  // namespace Core::Display