add_library(CommandEdit STATIC commandEdit.cpp commandEdit.hpp)

target_link_libraries(CommandEdit PUBLIC CommandFactory)