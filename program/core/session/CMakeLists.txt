add_library(Session STATIC session.cpp session.hpp)

add_subdirectory(data)
add_subdirectory(action)
add_subdirectory(requestResolver)

target_link_libraries(Session PUBLIC 
    WorkspaceManager
    SqlAdapter
    Logger
    Menu
    StringManip
    Action
    Data
    RequestResolver)