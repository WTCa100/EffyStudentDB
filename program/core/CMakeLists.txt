add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    SQLite::SQLite3
    Student
    Logger
    WorkspaceManager
    #TMP
    DirectoryManager
    FileManager
    )