#include "subject.hpp"

namespace Core::Types
{
    std::shared_ptr<Entry> Subject::fillGaps(const std::shared_ptr<Entry> other)
    {
        std::shared_ptr<Subject> concrete = std::static_pointer_cast<Subject>(other);
        std::shared_ptr<Subject> refObj   = std::make_shared<Subject>();
        refObj->id_                       = id_ == 0 ? concrete->id_ : id_;
        refObj->name_                     = name_.empty() ? concrete->name_ : name_;
        return refObj;
    }

    Entry& Subject::operator= (const Entry& other)
    {
        if (this == &other) { return *this; }

        const auto& otherSubject = dynamic_cast<const Subject&>(other);

        id_   = otherSubject.id_;
        name_ = otherSubject.name_;

        return *this;
    }
}  // namespace Core::Types
