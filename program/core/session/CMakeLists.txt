add_library(Session STATIC session.cpp session.hpp)

add_subdirectory(data)

target_link_libraries(Session PUBLIC 
    WorkspaceManager
    Logger
    Menu
    Data)