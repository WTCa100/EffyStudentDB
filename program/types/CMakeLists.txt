add_subdirectory(./course)
add_subdirectory(./school)
add_subdirectory(./student)
add_subdirectory(./studentRequest)
add_subdirectory(./subject)
add_subdirectory(./grade)
add_subdirectory(./courseSubjectWeight)

add_library(EntryFactory STATIC entryFactory.cpp entryFactory.hpp)
