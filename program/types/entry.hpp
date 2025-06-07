#pragma once

#include <inttypes.h>
#include <map>
#include <string>
#include <unordered_map>

namespace Core::Types
{

	struct Entry
	{
		uint16_t id_;
		const std::string associatedTable_;

		Entry(std::string table):
			id_(0),
			associatedTable_(table)
		{}

		Entry(uint16_t id, std::string table):
			id_(id),
			associatedTable_(table)
		{}

		virtual ~Entry() {};
		virtual std::string toString() const = 0;

		virtual std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) = 0;

		/// @brief This function shall return all of the SQL related attributes such as
		/// name, related IDs, float values etc. In most cases it will not include "id"
		/// @return Attributes mapped "name" -> "value"
		inline virtual std::map<std::string, std::string> getAttrs() const
		{
			return {
				{ "id", std::to_string(id_) }
			};
		};

		virtual Entry& operator= (const Entry& other) = 0;
	};

}  // namespace Core::Types
