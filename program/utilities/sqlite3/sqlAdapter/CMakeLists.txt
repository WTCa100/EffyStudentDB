add_library(SqlAdapter STATIC sqlAdapter.cpp sqlAdapter.hpp)

target_link_libraries(SqlAdapter PUBLIC 
    SqlManager
    Logger
    SqlTable
    SqlAttribute)
