#pragma once

#include <iostream>
#include <type_traits>

namespace sw
{
	class CommandParserVisitor
	{
	private:
		std::istream& _stream;
		const char* _badField = nullptr;

	public:
		CommandParserVisitor(std::istream& stream) :
				_stream(stream)
		{}

		template <class TField>
		void visit(const char* name, TField& field)
		{
			if (_badField != nullptr)
			{
				return;
			}
			// The stream silently wraps "-1" around to a huge unsigned value
			if constexpr (std::is_unsigned_v<TField>)
			{
				if ((_stream >> std::ws).peek() == '-')
				{
					_badField = name;
					return;
				}
			}
			if (!(_stream >> field))
			{
				_badField = name;
			}
		}

		// The first field that is missing or not a valid value, nullptr if all are fine
		const char* getBadField() const
		{
			return _badField;
		}
	};
}
