/*
 * Copyright © 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Robert Carr <robert.carr@canonical.com>
 */

#include "mir/event_type_to_string.h"
#include "mir/logging/logger.h"

#include "mir_toolkit/events/event.h"
#include "mir_toolkit/events/event_private.h"

#include "mir_toolkit/events/surface_event.h"
#include "mir_toolkit/events/resize_event.h"
#include "mir_toolkit/events/prompt_session_event.h"
#include "mir_toolkit/events/orientation_event.h"

#include <stdlib.h>

namespace ml = mir::logging;

namespace
{
template <typename EventType>
void expect_event_type(EventType const* ev, MirEventType t)
{
    if (ev->type != t)
    {
        ml::log(ml::Severity::critical, "Expected " + mir::event_type_to_string(t) + " but event is of type " +
            mir::event_type_to_string(ev->type));
    }
}
}

std::string mir::event_type_to_string(MirEventType t)
{
    switch (t)
    {
    case mir_event_type_key:
        return "mir_event_type_key";
    case mir_event_type_motion:
        return "mir_event_type_motion";
    case mir_event_type_surface:
        return "mir_event_type_surface";
    case mir_event_type_resize:
        return "mir_event_type_resize";
    case mir_event_type_prompt_session_state_change:
        return "mir_event_type_prompt_session_state_change";
    case mir_event_type_orientation:
        return "mir_event_type_orientation";
    case mir_event_type_close_surface:
        return "mir_event_type_close_surface";
    case mir_event_type_input:
        return "mir_event_type_input";
    default:
        abort();
    }
}


MirEventType mir_event_get_type(MirEvent const* ev)
{
    switch (ev->type)
    {
    case mir_event_type_key:
    case mir_event_type_motion:
        return mir_event_type_input;
    default:
        return ev->type;
    }
}

MirInputEvent const* mir_event_get_input_event(MirEvent const* ev)
{
    expect_event_type(ev, mir_event_type_input);

    return reinterpret_cast<MirInputEvent const*>(ev);
}

MirSurfaceEvent const* mir_event_get_surface_event(MirEvent const* ev)
{
    expect_event_type(ev, mir_event_type_surface);
    
    return &ev->surface;
}

MirResizeEvent const* mir_event_get_resize_event(MirEvent const* ev)
{
    expect_event_type(ev, mir_event_type_resize);
    
    return &ev->resize;
}

MirPromptSessionEvent const* mir_event_get_prompt_session_event(MirEvent const* ev)
{
    expect_event_type(ev, mir_event_type_prompt_session_state_change);
    
    return &ev->prompt_session;
}

MirOrientationEvent const* mir_event_get_orientation_event(MirEvent const* ev)
{
    expect_event_type(ev, mir_event_type_orientation);

    return &ev->orientation;
}

MirCloseSurfaceEvent const* mir_event_get_close_surface_event(MirEvent const* ev)
{
    expect_event_type(ev, mir_event_type_close_surface);

    return &ev->close_surface;
}

/* Surface event accessors */

MirSurfaceAttrib mir_surface_event_get_attribute(MirSurfaceEvent const* ev)
{
    expect_event_type(ev, mir_event_type_surface);

    return ev->attrib;
}

int mir_surface_event_get_attribute_value(MirSurfaceEvent const* ev)
{
    expect_event_type(ev, mir_event_type_surface);

    return ev->value;
}

/* Resize event accessors */

int mir_resize_event_get_width(MirResizeEvent const* ev)
{
    expect_event_type(ev, mir_event_type_resize);
    return ev->width;
}

int mir_resize_event_get_height(MirResizeEvent const* ev)
{
    expect_event_type(ev, mir_event_type_resize);
    return ev->height;
}

/* Prompt session event accessors */

MirPromptSessionState mir_prompt_session_event_get_state(MirPromptSessionEvent const* ev)
{
    expect_event_type(ev, mir_event_type_prompt_session_state_change);
    return ev->new_state;
}

/* Orientation event accessors */

MirOrientation mir_orientation_event_get_direction(MirOrientationEvent const* ev)
{
    expect_event_type(ev, mir_event_type_orientation);
    return ev->direction;
}
