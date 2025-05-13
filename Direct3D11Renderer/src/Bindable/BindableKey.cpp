#include "Bindable/BindableKey.h"

BindableKey::BindableKey(std::type_index type, const std::string& id)
	: type(type), id(id)
{
}

bool BindableKey::operator==(const BindableKey& rhs) const
{
	return type == rhs.type && id == rhs.id;
}
