from pathlib import Path

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