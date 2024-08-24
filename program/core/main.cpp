#include <iostream>
#include <vector>

// This is tmp!
#include "../utilities/workspaceManager/directoryManager/dirManager.hpp"
#include "../utilities/workspaceManager/fileManager/fManager.hpp"
#include "../utilities/workspaceManager/wsManager.hpp"
#include "../utilities/sqlite3/sqlManager/sqlManager.hpp"
#include <sqlite3.h>

using Utilities::Workspace::SqlManager;
int main(int argc, char const *argv[])
{
    SqlManager handler("Effy.db");
    std::vector<std::string> resultTest = handler.getEntriesFromTable("Students", {"id", "name"});

    for(const auto& e : resultTest)
    {
        std::cout << e << "\n";
    }
}
