/*
 * Copyright (C) 2018 Marius Gripsgard <marius@ubports.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 or 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MIR_FRONTEND_XWAYLAND_LOG_H
#define MIR_FRONTEND_XWAYLAND_LOG_H

#include <mir/log.h>
#include <stdlib.h>

namespace mir
{
inline void log_verbose(std::string const& message)
{
    if (getenv("MIR_X11_VERBOSE_LOG"))
        log_info(message);
}
template <typename... Args>
void log_verbose(char const* fmt, Args&&... args)
{
    if (getenv("MIR_X11_VERBOSE_LOG"))
        log_info(fmt, std::forward<Args>(args)...);
}
} /* mir */

#endif /* end of include guard: MIR_FRONTEND_XWAYLAND_LOG_H */
