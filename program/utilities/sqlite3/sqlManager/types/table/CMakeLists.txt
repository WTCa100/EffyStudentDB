add_library(SqlTable STATIC table.cpp table.hpp)

target_link_libraries(SqlTable PUBLIC SqlAttribute)