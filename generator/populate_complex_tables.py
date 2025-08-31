from common import find_database_location
import sqlite3
import json

import table_information

def load_courses(subjects: dict[str:table_information.Subject]):
    with open("constant_values/courses.json", "r") as json_ptr:
        json_content = json.load(json_ptr)
    courses : dict[str, table_information.Course] = {}
    for entry in json_content:
        current_course : table_information.Course = table_information.Course(0, 
                                                                             entry["name"], 
                                                                             entry["min_points"], 
                                                                             entry["max_students"],
                                                                             entry["min_students"],
                                                                             {})
        # Now assign weights to subjects. Target structure dict[subjectId: (subjectName, weight)]
        for subject_row in entry['subjects_required']:
            target_subject_name = subject_row['subject_name']
            target_subject_weight = subject_row['subject_weight']
            if target_subject_name in subjects.keys():
                actual_subject_id = subjects[target_subject_name].id
                print(f'For {current_course.name} assigned {target_subject_name} (id={actual_subject_id}) with weight = {target_subject_weight}')
                current_course.courseSubjectWeights[actual_subject_id] = (target_subject_name, target_subject_weight)
        courses[current_course.name] = current_course
    return courses

def insert_courses(cursor : sqlite3.Cursor, courses : dict[str, table_information.Course]):
    sql_insert_base = "INSERT INTO courses (name, baseMinimalPoints, maxStudents, minStudents, isOpen, recrutingTurn) VALUES "
    for course_name, course_entry in courses.items():
        sql_insert_value_row = f'("{course_name}", {course_entry.baseMinimalPoints}, {course_entry.maxStudents}, {course_entry.minStudents}, {True}, {1}), '
        sql_insert_base += sql_insert_value_row
    sql_insert_base = sql_insert_base[:-2] + ";"
    print(f"Executing: {sql_insert_base}")
    cursor.execute(sql_insert_base)

def assign_id_to_course(cursor: sqlite3.Cursor, courses: dict[str, table_information.Course]):
    sql_select = "SELECT id, name FROM courses;"
    print(f'Executing: {sql_select}')
    cursor.execute(sql_select)
    result = cursor.fetchall()
    for row in result:
        db_course_id = row[0]
        db_course_name = row[1]
        if db_course_name in courses:
            courses[db_course_name].id = db_course_id
            print(f'Assigned ID {db_course_id} to {db_course_name}')

def insert_weights(cursor: sqlite3.Cursor, courses: dict[str, table_information.Course]):
    sql_insert_base = "INSERT INTO courseSubjectWeight (courseId, subjectId, weight) VALUES "
    for course_name, course in courses.items():
        for subject_id, subject_name_and_weight in course.courseSubjectWeights.items():
            print(f'For {course_name}, assigning subject:weight pair = {subject_name_and_weight[0]}:{subject_name_and_weight[1]}')
            sql_insert_value_row = f"({course.id}, {subject_id}, {subject_name_and_weight[1]}), "
            sql_insert_base += sql_insert_value_row
    sql_insert_base = sql_insert_base[:-2] + ';'
    print(f'Executing {sql_insert_base}')
    cursor.execute(sql_insert_base)

def run(subjects: dict[str:table_information.Subject], schools: dict[str:table_information.School]):
    db = sqlite3.connect(find_database_location())
    cursor = db.cursor()
    
    print("Remove existing entries...")
    cursor.execute("DELETE FROM students")
    cursor.execute("DELETE FROM grades")
    cursor.execute("DELETE FROM courses")
    cursor.execute("DELETE FROM courseSubjectWeight")
    cursor.execute("DELETE FROM studentRequest")
    cursor.execute("DELETE FROM courseAttendees")
    db.commit()

    courses_info = load_courses(subjects)
    print(f"Got {len(courses_info)} courses!")
    insert_courses(cursor, courses_info)
    db.commit()
    assign_id_to_course(cursor, courses_info)
    insert_weights(cursor, courses_info)
    db.commit()
    
    
    
    return courses_info
    
if __name__ == "__main__":
    run([],[])