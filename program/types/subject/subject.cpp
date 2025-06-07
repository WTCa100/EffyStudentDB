#include "subject.hpp"

namespace Core::Types
{
	Entry& Subject::operator= (const Entry& other)
	{
		if (this == &other) { return *this; }

		const auto& otherSubject = dynamic_cast<const Subject&>(other);

		id_   = otherSubject.id_;
		name_ = otherSubject.name_;

		return *this;
	}
}  // namespace Core::Types
