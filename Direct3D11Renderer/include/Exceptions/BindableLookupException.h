#pragma once
#include <sstream>
#include <stdexcept>
#include "Exceptions/D3Exception.h"

class BindableLookupException : public D3Exception
{
public:
    BindableLookupException(int line, const char* file, const char* type, const std::string& id);

    const char* GetType() const noexcept override;

    const char* what() const noexcept override;

    const char* GetBindableType() const noexcept;

    const std::string& GetBindableId() const noexcept;

private:
    std::string type_name;
    std::string bindable_id;
};

// Define a macro for easier exception throwing
#define BINDABLE_LOOKUP_EXCEPT(type, id) BindableLookupException(__LINE__, __FILE__, type, id)
