/*
 * Copyright © 2015-2018 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3,
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
 * Authored by: Christopher James Halse Rogers <christopher.halse.rogers@canonical.com>
 */

#include "wl_subcompositor.h"
#include "wl_surface.h"

#include "mir/geometry/rectangle.h"

namespace mf = mir::frontend;
namespace geom = mir::geometry;

mf::WlSubcompositor::WlSubcompositor(struct wl_display* display)
    : wayland::Subcompositor(display, 1)
{}

void mf::WlSubcompositor::destroy(struct wl_client* /*client*/, struct wl_resource* resource)
{
    wl_resource_destroy(resource);
}

void mf::WlSubcompositor::get_subsurface(struct wl_client* client, struct wl_resource* resource, uint32_t id,
                                         struct wl_resource* surface, struct wl_resource* parent)
{
    new WlSubsurface(client, resource, id, WlSurface::from(surface), WlSurface::from(parent));
}

mf::WlSubsurface::WlSubsurface(struct wl_client* client, struct wl_resource* object_parent, uint32_t id,
                               WlSurface* surface, WlSurface* parent_surface)
    : wayland::Subsurface(client, object_parent, id),
      surface{surface},
      parent{parent_surface->add_child(this)},
      parent_destroyed{parent_surface->destroyed_flag()},
      synchronized_{true}
{
    surface->set_role(this);
    surface->pending_invalidate_surface_data();
}

mf::WlSubsurface::~WlSubsurface()
{
    // unique pointer automatically removes `this` from parent child list

    surface->clear_role();
    refresh_surface_data_now();
}

void mf::WlSubsurface::populate_surface_data(std::vector<shell::StreamSpecification>& buffer_streams,
                                             std::vector<mir::geometry::Rectangle>& input_shape_accumulator,
                                             geometry::Displacement const& parent_offset) const
{
    surface->populate_surface_data(buffer_streams, input_shape_accumulator, parent_offset);
}

bool mf::WlSubsurface::synchronized() const
{
    return synchronized_ || parent->synchronized();
}

mf::SurfaceId mf::WlSubsurface::surface_id() const
{
    return parent->surface_id();
}

void mf::WlSubsurface::parent_has_committed()
{
    if (cached_state && synchronized())
    {
        surface->commit(cached_state.value());
        cached_state = std::experimental::nullopt;
    }
}

mf::WlSurface::Position mf::WlSubsurface::transform_point(geom::Point point)
{
    return surface->transform_point(point);
}

void mf::WlSubsurface::set_position(int32_t x, int32_t y)
{
    surface->set_pending_offset(geom::Displacement{x, y});
}

void mf::WlSubsurface::place_above(struct wl_resource* sibling)
{
    (void)sibling;
    // TODO
}

void mf::WlSubsurface::place_below(struct wl_resource* sibling)
{
    (void)sibling;
    // TODO
}

void mf::WlSubsurface::set_sync()
{
    synchronized_ = true;
}

void mf::WlSubsurface::set_desync()
{
    synchronized_ = false;
}

void mf::WlSubsurface::destroy()
{
    wl_resource_destroy(resource);
}

void mf::WlSubsurface::refresh_surface_data_now()
{
    if (!*parent_destroyed)
        parent->refresh_surface_data_now();
}

void mf::WlSubsurface::commit(WlSurfaceState const& state)
{
    if (!cached_state)
        cached_state = WlSurfaceState();

    cached_state.value().update_from(state);

    if (synchronized())
    {
        if (cached_state.value().surface_data_needs_refresh() && !*parent_destroyed)
            parent->pending_invalidate_surface_data();
    }
    else
    {
        surface->commit(cached_state.value());
        if (cached_state.value().surface_data_needs_refresh())
            refresh_surface_data_now();
        cached_state = std::experimental::nullopt;
    }
}

void mf::WlSubsurface::visiblity(bool visible)
{
    (void)visible;
    // TODO
}
