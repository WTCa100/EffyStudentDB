add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(session)
add_subdirectory(menu)
add_subdirectory(displayHelper)

target_link_libraries(${PROJECT_NAME}
    SQLite::SQLite3
    Session
    WorkspaceManager
    )