#include "mdx.h"
#include "mdx_exception.h"

namespace mdx
{
    Exception::Exception(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        _vsnprintf_s(m_msg, _countof(m_msg), format, args);
        m_msg[_countof(m_msg) - 1] = 0;
        va_end(args);
    }
}