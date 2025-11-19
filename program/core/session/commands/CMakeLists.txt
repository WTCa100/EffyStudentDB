add_subdirectory(commandHelp)
add_subdirectory(commandAdd)
add_subdirectory(commandShow)
add_subdirectory(commandAssign)

add_library(CommandFactory STATIC CommandFactory.cpp CommandFactory.hpp)

target_link_libraries(CommandFactory PUBLIC 
                      CommandHelp
                      CommandAdd
                      CommandShow
                      CommandAssign)
