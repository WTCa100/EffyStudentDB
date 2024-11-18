add_library(WorkspaceManager STATIC wsManager.cpp wsManager.hpp)

add_subdirectory(directoryManager)
add_subdirectory(fileManager)
add_subdirectory(commandHandler)

target_link_libraries(WorkspaceManager PUBLIC 
    FileManager
    DirectoryManager
    SqlManager
    SqlAdapter
    Logger
    SqlTable
    SqlAttribute)
