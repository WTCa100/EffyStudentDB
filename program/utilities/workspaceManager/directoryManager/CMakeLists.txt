add_library(DirectoryManager STATIC dirManager.cpp dirManager.hpp)

target_link_libraries(DirectoryManager PUBLIC 
    CommandHandler
    CreateCommand
    RemoveCommand
    VerifyCommand)
