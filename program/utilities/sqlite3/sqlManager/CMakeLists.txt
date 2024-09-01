add_library(SqlManager STATIC sqlManager.cpp sqlManager.hpp)

add_subdirectory(types)

target_link_libraries(SqlManager PUBLIC
    SQLite::SQLite3
    Student 
    SqlTable
    SqlAttribute
    )