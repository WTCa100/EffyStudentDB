add_library(Course STATIC course.cpp course.hpp)

add_subdirectory(attendees)

target_link_libraries(Course PUBLIC CourseAttendees)