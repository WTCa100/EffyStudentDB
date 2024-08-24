add_library(SqlManager STATIC sqlManager.cpp sqlManager.hpp)


target_link_libraries(SqlManager PUBLIC SQLite::SQLite3)