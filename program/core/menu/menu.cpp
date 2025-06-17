#include "menu.hpp"

#include "../../utilities/common/stringManip.hpp"
#include "../../utilities/inputHandler/inputHandler.hpp"

namespace Core::Display
{
	Menu::Menu(std::shared_ptr<Utilities::Logger> logger, std::shared_ptr<SessionData> sesData)
	{
		logger_  = logger;
		sesData_ = sesData;
		LOG((*logger_), "Menu object created");
		inHandler_ = std::make_unique<Utilities::InputHandler>();
	}

	/* @TODO
		Add class `Action` that would be handling the user input and parsing, tokenizing it etc.
		Then return the new Action object, and let the session handle it
		Example:
		Enum Value ()
		Action:
			- Tokenized Action (vector of strings)
			- static Tokenize
			+ getAction (1st token)
			+ getTable (2nd token)
			+ getOptionalTags (3rd + tokens: as vector)
	*/
	Action Menu::manageDatabase()
	{
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
				if (cmd == "SHOW") { showEntries(userAction.getTarget()); }
				else if (cmd == "HELP") { showHelp(); }
				else if (cmd == "LIST") { listTables(); }
				LOG((*logger_), "Valid Action command: ", cmd, " target: ", userAction.getTarget());
			}
			else
			{
				LOG((*logger_), "Got invalid action. Cmd = ", cmd, " target = ", userAction.getTarget());
				std::cout << "Invalid action! Use \"HELP\" for available actions.\n";
			}
		} while ((cmd == "SHOW" || cmd == "HELP" || cmd == "LIST") || !isValidAction);
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
		helpMsg << "If an ID is provided it will only update that one entry.\n";
		helpMsg << "FIND <TargetTable> <TargetId - opt> - WIll launch a prompt and display every entry matching given pattern.\n";
		helpMsg << "If an ID is provided it will only display that one entry\n";
		helpMsg << "LIST - Will provide the user with the full list of table names.\n";
		helpMsg << "SHOW <TargetTable> - Will show entries from a given table.\n";
		helpMsg << "HELP - Displays this message.\n";
		std::cout << helpMsg.str();
		return;
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
		std::string rawInput = inHandler_->getStringBeauty("Instruction");
		LOG((*logger_), "Received input to assemble action: \"", rawInput, "\"");
		Action userAction = Action(inHandler_->toUpper(rawInput));
		return userAction;
	}

	void Menu::showEntries(const std::string& target) const
	{
		LOG((*logger_), "Showing entries from table: ", target);
		const std::shared_ptr<abstractTypeList> concreteList = sesData_->getEntries(target);

		if (!concreteList)
		{
			LOG((*logger_), "Could not find table: ", target);
			return;
		}

		std::cout << "Displaying " << target << ": \n";
		std::cout << concreteList->size() << " entries\n";
		for (const auto& entry : *concreteList) { std::cout << entry.second.get()->toString() << "\n"; }
	}

	bool Menu::validateAction(const Action& act)
	{
		const std::string& command = act.getCommand();

		if (command.empty())
		{
			LOG((*logger_), "Action command empty - aborting");
			return false;
		}

		// First check the shortest commands
		if (command == "LIST" || command == "HELP" || command == "EXIT") { return true; }

		if (command != "SHOW" && command != "FIND" && command != "ADD" && command != "REMOVE" && command != "UPDATE")
		{
			return false;
		}

		const std::string& target = act.getTarget();
		// Look for 2nd arg
		if (target.empty())
		{
			LOG((*logger_), "Action target empty while command == ", command);
			return false;
		}

		// 2nd Arg is always a table name
		if (!sesData_->getTableNames().contains(target))
		{
			LOG((*logger_), "Attempted to operate on non-existent target table: ", target);
			return false;
		}

		// @TODO add optional handle
		return true;
	}

	bool Menu::promptDeleteAll(std::string filter, uint16_t count) const
	{
		LOG((*logger_), "Delete all detected - double check prompt");
		std::cout << "You are about to delete every entry (" << count << ") matching filter: \"" << filter << "\".\n";
		std::cout << "Are you sure? - this cannot be undone.\n";
		if (inHandler_->getYesOrNo() == 'Y')
		{
			LOG((*logger_), "User agreed");
			std::cout << "Agreed\n";
			return true;
		}
		LOG((*logger_), "User aborted")
		std::cout << "Abort\n";
		return false;
	}

	std::string Menu::getManagementOption() const
	{
		LOG((*logger_), "Getting management option");
		std::cout << "Options:\n";
		std::cout << "NEXT - move 1 page fruther\n";
		std::cout << "PREV - move 1 page back\n";
		std::cout << "ADD - proceed to create a new entry\n";
		std::cout << "ALTER <targetID> - proceed to update entry with provided targetId with new values\n";
		std::cout << "REMOVE <targetID> - proceed to delete a given entry\n";
		std::cout << "FIND - proceed to get entries that match at least one parameter\n";
		std::cout << "EXIT - exits this view\n";
		std::string buf;
		std::getline(std::cin, buf);
		return buf;
	}

	MainMenuOption Menu::showMainMenu()
	{
		MainMenuOption op;
		LOG((*logger_), "Showing main menu");
		std::cout << "-=# Main Menu #=-\n";
		std::cout << "1. Manage Database\n";
		std::cout << "2. Handle student requests\n";
		std::cout << "3. Exit.\n";
		op = static_cast<MainMenuOption>(inHandler_->getOption(1, 3));
		LOG((*logger_), "Got option ", op);
		return op;
	}
}  // namespace Core::Display