from common import find_database_location
import sqlite3
import json
import random
import enum

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

def assign_id_to_courses(cursor: sqlite3.Cursor, courses: dict[str, table_information.Course]):
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

def load_students(schools: dict[str:table_information.School]):
    class gender(enum.IntEnum):
        female = 0,
        male   = 1,
    first_name_options_male = []
    first_name_option_female = []
    last_name_options = []
    email_domain_options = []
    with open('constant_values/polish_male_names.txt', "r") as fp:
        raw_names = fp.readline().split(',')
        first_name_options_male = [name.strip() for name in raw_names]
    with open('constant_values/polish_female_names.txt', "r") as fp:
        raw_names = fp.readline().split(',')
        first_name_option_female = [name.strip() for name in raw_names]
    with open('constant_values/student_middle_names.txt', "r") as fp:
        raw_surnames = fp.readline().split(',')
        last_name_options = [surname.strip() for surname in raw_surnames]
    with open('constant_values/email_domains.txt', "r") as fp:
        raw_emails = fp.readline().split(',')
        email_domain_options = [email.strip() for email in raw_emails]
    print(f'Got {len(first_name_options_male)} male names, {(len(first_name_option_female))} female names, and {len(last_name_options)} surnames!')
    print('Generating 1000 students...')
    students : dict[str: table_information.Student] = {}
    for i in range(0, 1000):
        gender_decision = random.randint(0,1)
        if gender_decision == gender.male:
            first_name = first_name_options_male[random.randrange(0, len(first_name_options_male))]
            second_name = first_name_options_male[random.randrange(0, len(first_name_options_male))] if random.randint(0, 1) else None
        else:
            first_name = first_name_option_female[random.randrange(0, len(first_name_option_female))]
            second_name = first_name_option_female[random.randrange(0, len(first_name_option_female))] if random.randint(0, 1) else None

        last_name = last_name_options[random.randrange(0, len(last_name_options))]

        # Generate email
        email = ""
        email = email + first_name[:3] + "_" + last_name[:3] + "_" + str(random.randint(1995, 2006)) + "@" + email_domain_options[random.randrange(0, len(email_domain_options))]
        # Choose random school
        selected_school_name, selected_school = random.choice(list(schools.items()))
        print(f'Generated values: {first_name=}, {second_name=}, {last_name=}, {email=}, {selected_school_name=}')
        students[email] = table_information.Student(0, first_name, second_name if second_name is not None else "", last_name, email, (selected_school, selected_school.id), {})
    print(f"Done generating 1000 students!")
    return students

def insert_students(cursor: sqlite3.Cursor, students: dict[str: table_information.Student]):
    # Each insert has to be done separately as the ammount of students with no second name is random each run.
    count = 0
    for student_email, student in students.items():
        progress = ((count+1) / (len(students))) * 100
        print(f'Inserting {count+1}/{len(students)} ({progress:.2f}%)')
        sql_insert = "INSERT INTO students (firstName, "
        sql_insert += "secondName, " if len(student.secondName) > 0 else ""
        sql_insert += "lastName, email, schoolId) VALUES"
        sql_insert += f'("{student.firstName}", '
        sql_insert += f'"{student.secondName}", ' if len(student.secondName) > 0 else ""
        sql_insert += f'"{student.lastName}", "{student_email}", {student.school[1]});'
        print(f'Executing: {sql_insert}')
        cursor.execute(sql_insert)
        count += 1

def assign_id_to_students(cursor: sqlite3.Cursor, students: dict[str, table_information.Student]):
    sql_select = "SELECT id, email FROM students;"
    print(f"Executing {sql_select}")
    cursor.execute(sql_select)
    result = cursor.fetchall()
    for row in result:
        db_student_id = row[0]
        db_student_email = row[1]
        if db_student_email in students:
            students[db_student_email].id = db_student_id
            print(f'Assigned ID {db_student_id} for student "{students[db_student_email].email}"')

def insert_grades(cursor: sqlite3.Cursor, students: dict[str, table_information.Student], subjects: dict[str,table_information.Subject]):
    print("Assigning grades to all subjects for all students - Only one rule, there cannot be any 1 as a grade value.")
    for student_email, student in students.items():
        # Grades for now can be from 2 to 10
        for subject_name, subject in subjects.items():
            generated_grade = random.randint(2, 10)
            print(f'For {student_email} grade for {subject_name} is set to {generated_grade}')
            student.grades[subject.id] = generated_grade
            sql_grade_insert = f"INSERT INTO grades (subjectId, studentId, grade) VALUES ({subject.id}, {student.id}, {generated_grade});"
            print(f'Executing {sql_grade_insert}')
            cursor.execute(sql_grade_insert)
            # TODO: Insert only canonical grades and then randomly choose:
            # 1. Foreign language
            # 2. Non language related fields
            # 3. Skip the rest

def select_unique_courses(courses: dict[str, table_information.Course], count = 3) -> set[table_information.Course]:
    print(f'Catching {count} courses')
    # Verify lenght 
    if(len(courses) < count):
        raise RuntimeError(f"Count {count} cannot be greater than courses count {len(courses)}!")
    selected_courses : set = set()
    while len(selected_courses) != count:
        selected_courses.add(courses[random.randrange(1, len(courses) - 1)])
    return selected_courses

# This will be random so don't be surprised seeing someone doing poorly on any STEM subjects and applying for STEM field
def send_requests(cursor: sqlite3.Cursor, students: dict[str, table_information.Student], courses: dict[int, table_information.Course]):
    print('Sending requests for students.')
    # Each student will send 3 requests for courses
    for student_email, student_obj in students.items():
        target_courses = select_unique_courses(courses)
        sql_insert = f'INSERT INTO studentrequest (studentId, courseId, requestStatus) VALUES '
        for course in target_courses:
            sql_insert += f'({student_obj.id}, {course.id}, "Pending"),'
        sql_insert = sql_insert[:-1] + ';'
        print(f'Handling: {student_email}: courses {[c.name for c in target_courses]}')
        print(f'Executing: {sql_insert}')
        cursor.execute(sql_insert)

def complex_type_key_swap_to_id(values: dict):
    return_iterable : dict = {}
    for value_key, value_obj in values.items():
        return_iterable[value_obj.id] = value_obj
    return return_iterable

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
    assign_id_to_courses(cursor, courses_info)
    insert_weights(cursor, courses_info)
    db.commit()
    
    students_info = load_students(schools)
    insert_students(cursor, students_info)
    db.commit()
    assign_id_to_students(cursor, students_info)
    insert_grades(cursor, students_info, subjects)
    db.commit()
    
    courses = complex_type_key_swap_to_id(courses_info)
    send_requests(cursor, students_info, courses)
    db.commit()
    
    return courses_info, students_info
    
if __name__ == "__main__":
    run({},{})
