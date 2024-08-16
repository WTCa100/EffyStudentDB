#include <iostream>
#include <vector>

// This is tmp!
#include "../utilities/workspaceManager/directoryManager/dirManager.hpp"
#include <sqlite3.h>

using Utilities::Workspace::DirectoryManager;
int main(int argc, char const *argv[])
{
    DirectoryManager test1;

    const auto& b = test1.getAppRoot();
    std::cout << b.string() << "\n";


    DirectoryManager test2("program/core/");
    std::cout <<  test1.isDirectoryValid("program/core/") << "\n";
    std::cout << test2.isDirectoryValid(test1.getAppRootStr()) << "\n";

    // if(test1.createDirectory("HotDog"))
    // {
    // }


    return 0;
}
