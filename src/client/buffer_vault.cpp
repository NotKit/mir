/*
 * Copyright © 2015 Canonical Ltd.
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
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */

#include "mir/client_buffer_factory.h"
#include "mir/client_buffer.h"
#include "buffer_vault.h"
#include "mir_protobuf.pb.h"
#include "protobuf_to_native_buffer.h"
#include <algorithm>
#include <boost/throw_exception.hpp>

namespace mcl = mir::client;
namespace mp = mir::protobuf;
namespace geom = mir::geometry;
namespace mp = mir::protobuf;

enum class mcl::BufferVault::Owner
{
    Server,
    ContentProducer,
    Self
};

mcl::BufferVault::BufferVault(
    std::shared_ptr<ClientBufferFactory> const& client_buffer_factory,
    std::shared_ptr<ServerBufferRequests> const& server_requests,
    std::shared_ptr<SurfaceMap> const& map,
    std::shared_ptr<AsyncBufferFactory> const& mirfactory,
    geom::Size size, MirPixelFormat format, int usage, unsigned int initial_nbuffers) :
    factory(client_buffer_factory),
    server_requests(server_requests),
    map(map), mirfactory(mirfactory),
    format(format),
    usage(usage),
    size(size),
    disconnected_(false)
{
    for (auto i = 0u; i < initial_nbuffers; i++)
        server_requests->allocate_buffer(size, format, usage);
}

mcl::BufferVault::~BufferVault()
{
    if (disconnected_)
        return;

    for (auto& it : buffers)
    try
    {
        server_requests->free_buffer(it.first);
    }
    catch (...)
    {
    }
}

mcl::NoTLSFuture<mcl::BufferInfo> mcl::BufferVault::withdraw()
{
    std::lock_guard<std::mutex> lk(mutex);
    mcl::NoTLSPromise<mcl::BufferInfo> promise;
    auto it = std::find_if(buffers.begin(), buffers.end(),
        [this](std::pair<int, BufferEntry> const& entry) { 
            return ((entry.second.owner == Owner::Self) && (size == entry.second.buffer->size())); });

    auto future = promise.get_future();
    if (it != buffers.end())
    {
        it->second.owner = Owner::ContentProducer;
        promise.set_value({it->second.buffer, it->first});
    }
    else
    {
        promises.emplace_back(std::move(promise));
    }
    return future;
}

void mcl::BufferVault::deposit(std::shared_ptr<mcl::ClientBuffer> const& buffer)
{
    std::lock_guard<std::mutex> lk(mutex);
    auto it = std::find_if(buffers.begin(), buffers.end(),
        [&buffer](std::pair<int, BufferEntry> const& entry) { return buffer == entry.second.buffer; });
    if (it == buffers.end() || it->second.owner != Owner::ContentProducer)
        BOOST_THROW_EXCEPTION(std::logic_error("buffer cannot be deposited"));

    it->second.owner = Owner::Self;
    it->second.buffer->increment_age();
}

void mcl::BufferVault::wire_transfer_outbound(std::shared_ptr<mcl::ClientBuffer> const& buffer)
{
    int id;
    std::shared_ptr<mcl::ClientBuffer> submit_buffer;
    std::unique_lock<std::mutex> lk(mutex);
    auto it = std::find_if(buffers.begin(), buffers.end(),
        [&buffer](std::pair<int, BufferEntry> const& entry) { return buffer == entry.second.buffer; });
    if (it == buffers.end() || it->second.owner != Owner::Self)
        BOOST_THROW_EXCEPTION(std::logic_error("buffer cannot be transferred"));

    it->second.owner = Owner::Server;
    it->second.buffer->mark_as_submitted();
    submit_buffer = it->second.buffer;
    id = it->first;
    lk.unlock();
    server_requests->submit_buffer(id, *submit_buffer);
}

void mcl::BufferVault::wire_transfer_inbound(mp::Buffer const& protobuf_buffer)
{
    std::shared_ptr<MirBufferPackage> package = mcl::protobuf_to_native_buffer(protobuf_buffer);

    std::unique_lock<std::mutex> lk(mutex);
    auto it = buffers.find(protobuf_buffer.buffer_id());
    if (it == buffers.end())
    {
        geom::Size sz{package->width, package->height};
        if (sz != size)
        {
            lk.unlock();
            server_requests->free_buffer(protobuf_buffer.buffer_id());
            for (int i = 0; i != package->fd_items; ++i)
                close(package->fd[i]);

            server_requests->allocate_buffer(size, format, usage);
            return;
        }

        buffers[protobuf_buffer.buffer_id()] = 
            BufferEntry{ factory->create_buffer(package, sz, format), Owner::Self };
    }
    else
    {
        it->second.buffer->update_from(*package);
        if (size == it->second.buffer->size())
        { 
            it->second.owner = Owner::Self;
        }
        else
        {
            int id = it->first;
            buffers.erase(it);
            lk.unlock();
            server_requests->free_buffer(id);
            server_requests->allocate_buffer(size, format, usage);
            return;
        }
    }

    if (!promises.empty())
    {
        buffers[protobuf_buffer.buffer_id()].owner = Owner::ContentProducer;
        promises.front().set_value({buffers[protobuf_buffer.buffer_id()].buffer, protobuf_buffer.buffer_id()});
        promises.pop_front();
    }
}

//TODO: the server will currently spam us with a lot of resize messages at once,
//      and we want to delay the IPC transactions for resize. If we could rate-limit
//      the incoming messages, we should should consolidate the scale and size functions
void mcl::BufferVault::set_size(geom::Size sz)
{
    std::lock_guard<std::mutex> lk(mutex);
    size = sz;
}

void mcl::BufferVault::disconnected()
{
    std::lock_guard<std::mutex> lk(mutex);
    disconnected_ = true;
    promises.clear();
}

void mcl::BufferVault::set_scale(float scale)
{
    std::vector<int> free_ids;
    std::unique_lock<std::mutex> lk(mutex);
    auto new_size = size * scale;
    if (new_size == size)
        return;
    size = new_size;
    for (auto it = buffers.begin(); it != buffers.end();)
    {
        if ((it->second.owner == Owner::Self) && (it->second.buffer->size() != size)) 
        {
            free_ids.push_back(it->first);
            it = buffers.erase(it);
        }
        else
        {
            it++;
        }
    } 
    lk.unlock();

    for(auto& id : free_ids)
    {
        server_requests->allocate_buffer(new_size, format, usage);
        server_requests->free_buffer(id);
    }
}
