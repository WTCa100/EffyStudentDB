from dataclasses import dataclass

@dataclass
class Course:
    id : int
    name : str
    baseMinimalPoints : int
    maxStudents : int
    minStudents : int
    courseSubjectWeights : dict[int:tuple[str, float]] # ID + Value

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
    # secondName : str | None
    lastName : str
    email : str