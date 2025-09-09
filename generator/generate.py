import populate_basic_tables as generator_basic
import populate_complex_tables as generator_complex

if __name__ == "__main__":
    schools, subjects = generator_basic.run()
    print(f'Returned {len(subjects)} subjects and {len(schools)} schools')
    generator_complex.run(schools, subjects)
