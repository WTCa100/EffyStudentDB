import populate_basic_tables as generator_basic
import json
import sqlite3


if __name__ == "__main__":
    schools, subjects = generator_basic.run()
    print(f'Returned {len(subjects)} subjects and {len(schools)} schools')