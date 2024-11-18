add_library(Menu STATIC menu.cpp menu.hpp)

target_link_libraries(Menu PUBLIC
    Data
    InputHandler
)