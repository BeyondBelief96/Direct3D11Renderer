#include "Exceptions/D3Exception.h"
#include <sstream>

D3Exception::D3Exception(int line, const char* file) noexcept
	: line(line), file(file)
{
}

const char* D3Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* D3Exception::GetType() const noexcept
{
	return "D3Exception";
}

int D3Exception::GetLine() const noexcept
{
	return line;
}

const std::string& D3Exception::GetFile() const noexcept
{
	return file;
}

std::string D3Exception::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << GetFile() << std::endl
		<< "[Line] " << GetLine() << std::endl;
	return oss.str();
}
