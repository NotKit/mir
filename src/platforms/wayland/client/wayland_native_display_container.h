/*
 * Copyright © 2017 The Ubports project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by:
 *   Marius Gripsgard <marius@ubports.com>
 */

#ifndef MIR_CLIENT_WAYLAND_NATIVE_DISPLAY_CONTAINER_H_
#define MIR_CLIENT_WAYLAND_NATIVE_DISPLAY_CONTAINER_H_

#include "mir/egl_native_display_container.h"

#include "mir_toolkit/client_types.h"

#include <unordered_set>
#include <mutex>

namespace mir
{
namespace client
{
namespace wayland
{

class WaylandNativeDisplayContainer : public EGLNativeDisplayContainer
{
public:
    WaylandNativeDisplayContainer();
    virtual ~WaylandNativeDisplayContainer();

    MirEGLNativeDisplayType create(ClientPlatform* platform);
    void release(MirEGLNativeDisplayType display) override;

    bool validate(MirEGLNativeDisplayType display) const override;

protected:
    WaylandNativeDisplayContainer(WaylandNativeDisplayContainer const&) = delete;
    WaylandNativeDisplayContainer& operator=(WaylandNativeDisplayContainer const&) = delete;

private:
    std::mutex mutable guard;
    std::unordered_set<MirEGLNativeDisplayType> valid_displays;
};

}
}
} // namespace mir

#endif // MIR_CLIENT_WAYLAND_NATIVE_DISPLAY_CONTAINER_H_
