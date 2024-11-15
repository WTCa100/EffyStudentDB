add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(session)

target_link_libraries(${PROJECT_NAME}
    SQLite::SQLite3
    Session
    WorkspaceManager
    )