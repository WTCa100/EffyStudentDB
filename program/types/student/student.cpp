#include "student.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

#include <iostream>
#include <sstream>

namespace Core::Types
{
	std::string Student::toString(bool showGrades) const
	{
		std::stringstream ss("");
		ss << toString() << "\n";
		if (showGrades)
		{
			if (!grades_.empty())
			{
				ss << ", Grades: \n";
				for (const auto& grade : grades_) { ss << grade.first << ":" << grade.second << "\n"; }
			}
			else { ss << "No grades."; }
		}
		return ss.str();
	}

	std::string Student::toString() const
	{
		std::stringstream ss("");
		ss << "Id: " << id_ << ", First name: " << firstName_ << ", ";
		if (secondName_.has_value() && !secondName_.value().empty()) { ss << "Second name: " << secondName_.value() << ", "; }
		ss << "Last name: " << lastName_ << ", ";
		ss << "Email: " << email_ << ", ";
		ss << "Associated School: " << schoolId_ << " ";
		return ss.str();
	}

	std::map<std::string, std::string> Student::getAttrs() const
	{
		std::map<std::string, std::string> mappedAttrs;
		mappedAttrs.insert(std::make_pair("firstName", firstName_));
		if (secondName_.has_value()) mappedAttrs.insert(std::make_pair("secondName", secondName_.value()));
		mappedAttrs.insert(std::make_pair("lastName", lastName_));
		mappedAttrs.insert(std::make_pair("email", email_));
		mappedAttrs.insert(std::make_pair("schoolId", std::to_string(schoolId_)));
		return mappedAttrs;
	}

	std::unordered_map<std::string, std::string> Student::userConstruct(bool makeFull)
	{
		std::unordered_map<std::string, std::string> mappedNewAttrs;
		std::cout << "Creating student from user input\n";
		std::cout << "(Attributes tagged as 'optional' can be left empty)\n";
		firstName_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("First name")
							  : Utilities::InputHandler::getAttrAsString("First name");
		if (!firstName_.empty()) mappedNewAttrs.insert(std::make_pair("firstName", firstName_));

		secondName_ = Utilities::InputHandler::getAttrAsString("Second name (optional)");
		if (secondName_.has_value() && secondName_.value().empty()) { secondName_ = std::nullopt; }

		lastName_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("Last name")
							 : Utilities::InputHandler::getAttrAsString("Last name");
		if (!lastName_.empty()) mappedNewAttrs.insert(std::make_pair("lastName", lastName_));

		email_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("Email")
						  : Utilities::InputHandler::getAttrAsString("Email");
		if (!email_.empty()) mappedNewAttrs.insert(std::make_pair("email", email_));

		int tmpSchoolId = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Associated School (ID)")
								   : Utilities::InputHandler::getAttrAsNumber("Associated School (ID)");
		schoolId_       = tmpSchoolId >= 0 ? tmpSchoolId : 0;
		if (schoolId_ != 0) mappedNewAttrs.insert(std::make_pair("schoolId", std::to_string(schoolId_)));

		return mappedNewAttrs;
	}

	Student::Student(uint16_t id,
		std::string name,
		std::string lastName,
		std::string email,
		uint16_t schoolId,
		std::optional<std::string> secondName):
		Entry(id_, g_tableStudents),
		firstName_(name),
		secondName_(secondName),
		lastName_(lastName),
		email_(email),
		grades_({}),
		schoolId_(schoolId)
	{}

	Student::Student(
		std::string name, std::string lastName, std::string email, uint16_t schoolId, std::optional<std::string> secondName):
		Entry(0, g_tableStudents),
		firstName_(name),
		secondName_(secondName),
		lastName_(lastName),
		email_(email),
		grades_({}),
		schoolId_(schoolId)
	{}

	Entry& Student::operator= (const Entry& other)
	{
		if (this == &other) return *this;

		const auto& otherStudent = dynamic_cast<const Student&>(other);
		id_                      = other.id_;
		firstName_               = otherStudent.firstName_;
		secondName_              = otherStudent.secondName_;
		lastName_                = otherStudent.lastName_;
		email_                   = otherStudent.email_;
		grades_                  = otherStudent.grades_;
		schoolId_                = otherStudent.schoolId_;

		return *this;
	}

}  // namespace Core::Types
