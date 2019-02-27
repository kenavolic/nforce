#include "nforce/core/except.h"

namespace n4 {
//-------------------------------------
// Public

nexcept::nexcept(const std::string &str, status_type s)
    : std::exception{}, m_status{s} {}

status_type nexcept::status() const noexcept { return m_status; }
} // namespace n4