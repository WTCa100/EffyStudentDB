from pathlib import Path
import sqlite3

import table_information

def find_database_location():
    print("Looking for Effy.db...")
    cwd = Path(".").absolute()
    db_candidates = list(cwd.parent.rglob("*Effy.db"))
    if len(db_candidates) == 0:
        raise RuntimeError("Found no file nammed Effy.db...")
    elif len(db_candidates) != 1:
        raise RuntimeError("Found more than one file with Effy.db as a suffix")
    else:
        print(f"Found Effy.db at {db_candidates[0]}")
        return db_candidates[0]

def load_basic_information(table_name : str):
    with open(f"constant_values/{table_name}.txt", "r") as fp:
        return fp.read().split(',')

def load_subjects_information():
    raw_subjects = load_basic_information('subjects')
    subjects : list = []
    for subject in raw_subjects:
        subjects.append(table_information.Subject(0, subject.strip()))
    return subjects

def assign_id_to_basic_type(cursor : sqlite3.Cursor, table : str , entires: list):
    sql_select = f"SELECT name, id FROM {table};"
    print(f'Executing {sql_select}')
    cursor.execute(sql_select)
    result = cursor.fetchall()
    for db_entry, entry in zip(result, entires):
        if db_entry[0] == entry.name:
            entry.id = db_entry[1]

def load_schools_information():
    raw_schools = load_basic_information('schools')
    schools : list = []
    for school in raw_schools:
        schools.append(table_information.School(0, school.strip()))
    return schools

def insert_base_information(cursor : sqlite3.Cursor, table : str, values : list):
    sql_insert_base = f'INSERT INTO {table} (name) VALUES '
    for value in values:
        value_batch = f'("{value.name}"), '
        sql_insert_base += value_batch
    sql_insert_base = sql_insert_base[:-2] + ';'
    print(f"Executing: {sql_insert_base}")
    cursor.execute(sql_insert_base)

def basic_type_list_to_dict(values: list):
    return_iterable : dict = {}
    for value in values:
        return_iterable[value.id] = value
    return return_iterable

def run():
    db = sqlite3.connect(find_database_location())
    cursor = db.cursor()
    # Constant tables are: Schools, Subjects and Courses along with their scoring policies
    print('Clear prevoius entries')
    cursor.execute("DELETE FROM subjects;")
    cursor.execute("DELETE FROM schools;")
    
    subjects_info = load_subjects_information()
    print(f'Got {len(subjects_info)} subjects.')
    insert_base_information(cursor, "subjects", subjects_info)
    db.commit()
    assign_id_to_basic_type(cursor, "subjects", subjects_info)
    
    schools_info = load_schools_information()
    print(f'Got {len(schools_info)} schools.')
    insert_base_information(cursor, "schools", schools_info)
    db.commit()
    assign_id_to_basic_type(cursor, "schools", schools_info)
    
    cursor.close()
    db.close()
    # Turn into dict for easier look-up
    subjects = basic_type_list_to_dict(subjects_info)
    schools = basic_type_list_to_dict(schools_info)
    return subjects, schools

if __name__ == "__main__":
    subjects, schools = run()
    print(f'Returned {len(subjects)} subjects and {len(schools)} schools')