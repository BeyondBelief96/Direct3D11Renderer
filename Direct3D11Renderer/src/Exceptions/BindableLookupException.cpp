#include "Exceptions/BindableLookupException.h"

BindableLookupException::BindableLookupException(int line, const char* file, const char* type, const std::string& id)
    : D3Exception(line, file)
{
    type_name = type;
    bindable_id = id;
}

const char* BindableLookupException::GetType() const noexcept
{
    return "Bindable Lookup Exception";
}

const char* BindableLookupException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Type] " << type_name << std::endl
        << "[ID] " << bindable_id << std::endl
        << "[Error] Failed to find bindable in cache" << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* BindableLookupException::GetBindableType() const noexcept
{
    return type_name.c_str();
}

const std::string& BindableLookupException::GetBindableId() const noexcept
{
    return bindable_id;
}
