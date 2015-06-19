/*
 * Copyright © 2013 Canonical Ltd.
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
 * Authored by: Robert Carr <robert.carr@canonical.com>
 */

#ifndef MIR_EXAMPLES_DEFAULT_PLACEMENT_STRATEGY_H_
#define MIR_EXAMPLES_DEFAULT_PLACEMENT_STRATEGY_H_

#include "mir/scene/placement_strategy.h"

#include <memory>

namespace mir
{
namespace shell
{
class DisplayLayout;
}

namespace examples
{
class DefaultPlacementStrategy : public scene::PlacementStrategy
{
public:
    explicit DefaultPlacementStrategy(
            std::shared_ptr<shell::DisplayLayout> const& display_layout);
    virtual ~DefaultPlacementStrategy() {}

    virtual scene::SurfaceCreationParameters place(scene::Session const& session, scene::SurfaceCreationParameters const& request_parameters);

protected:
    DefaultPlacementStrategy(DefaultPlacementStrategy const&) = delete;
    DefaultPlacementStrategy& operator=(DefaultPlacementStrategy const&) = delete;

private:
    std::shared_ptr<shell::DisplayLayout> const display_layout;
};

}
} // namespace mir

#endif // MIR_SHELL_DEFAULT_PLACEMENT_STRATEGY_H_