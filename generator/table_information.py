from dataclasses import dataclass

@dataclass
class Course:
    id : int
    name : str
    baseMinimalPoints : int
    maxStudents : int
    minStudents : int
    courseSubjectWeights : dict[int:tuple[str, float]] # ID + Value
    def __hash__(self):
        return hash((self.id, self.name))

@dataclass
class Subject:
    id : int
    name : str

@dataclass
class School:
    id : int
    name : str

@dataclass
class Student:
    id : int
    firstName : str
    secondName : str
    lastName : str
    email : str
    school : tuple[School, int]
    grades : dict[id: int]
