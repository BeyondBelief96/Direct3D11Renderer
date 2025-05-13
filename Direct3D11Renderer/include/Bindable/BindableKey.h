#pragma once

#include "Bindable.h"
#include "Graphics.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <typeindex>

class BindableKey
{
public:
	BindableKey(std::type_index type, const std::string& id);
	bool operator==(const BindableKey& rhs) const;

	struct Hash
	{
		size_t operator()(const BindableKey& key) const
		{
			return std::hash<std::type_index>()(key.type) ^ std::hash<std::string>()(key.id);
		}
	};

private:
	std::type_index type;
	std::string id;
};
