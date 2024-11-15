add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(session)
add_subdirectory(menu)

target_link_libraries(${PROJECT_NAME}
    SQLite::SQLite3
    Session
    WorkspaceManager
    )