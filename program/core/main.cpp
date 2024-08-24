#include <iostream>
#include <vector>

// This is tmp!
#include "../utilities/workspaceManager/directoryManager/dirManager.hpp"
#include "../utilities/workspaceManager/fileManager/fManager.hpp"
#include "../utilities/workspaceManager/wsManager.hpp"
#include <sqlite3.h>

using Utilities::WsManager;
int main(int argc, char const *argv[])
{
    WsManager test;
    test.createDirectory("Test");
    test.createDirectory("SubDir", "Test");
    test.createFile("test.mp4", "Test");
    test.createFile("test2.mp3", "Test");

    // test.deleteFile("test.mp4", "Test/");
    test.deleteDirectory("Test");
}
