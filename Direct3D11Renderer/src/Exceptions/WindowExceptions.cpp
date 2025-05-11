#include "Exceptions/WindowExceptions.h"

const char* WindowHrException::GetType() const noexcept
{
    return "Window Exception";
}

const char* WindowNoGfxException::GetType() const noexcept
{
    return "Window Exception [No Graphics]";
}