add_library(FileManager STATIC fManager.cpp fManager.hpp)

target_link_libraries(FileManager PUBLIC 
    CommandHandler
    CreateCommand
    RemoveCommand
    VerifyCommand)