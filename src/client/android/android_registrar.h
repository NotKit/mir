/*
 * Copyright © 2012 Canonical Ltd.
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
 * Authored by:
 *   Kevin DuBois <kevin.dubois@canonical.com>
 */

#ifndef MIR_CLIENT_ANDROID_ANDROID_REGISTRAR_H_
#define MIR_CLIENT_ANDROID_ANDROID_REGISTRAR_H_

#include "mir/geometry/rectangle.h"
#include <cutils/native_handle.h>

#include <memory>
namespace mir
{
namespace client
{
class MemoryRegion;

namespace android
{
class AndroidRegistrar
{
public:
    virtual ~AndroidRegistrar() = default;

    virtual void register_buffer(const native_handle_t *handle) = 0;
    virtual void unregister_buffer(const native_handle_t *handle) = 0;
    virtual std::shared_ptr<char> secure_for_cpu(std::shared_ptr<const native_handle_t> handle, const geometry::Rectangle) = 0;
};

}
}
}
#endif /* MIR_CLIENT_ANDROID_REGISTRAR_H_ */
