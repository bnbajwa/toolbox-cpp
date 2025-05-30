// The Reactive C++ Toolbox.
// Copyright (C) 2013-2019 Swirly Cloud Limited
// Copyright (C) 2022 Reactive Markets Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <toolbox/http.hpp>
#include <toolbox/io.hpp>
#include <toolbox/sys.hpp>
#include <toolbox/util.hpp>

#include <map>

using namespace std;
using namespace toolbox;

namespace {

void on_foo(const Request& /*req*/, http::OStream& os)
{
    os << "Hello, Foo!";
}

void on_bar(const Request& /*req*/, http::OStream& os)
{
    os << "Hello, Bar!";
}

class ExampleApp final : public App {
  public:
    using Slot = BasicSlot<void(const Request&, http::OStream&)>;
    using SlotMap = std::map<std::string, Slot>;

    ~ExampleApp() override = default;
    void bind(const std::string& path, Slot slot) { slot_map_[path] = slot; }

  protected:
    void do_on_http_connect(CyclTime /*now*/, const Endpoint& ep) noexcept override
    {
        TOOLBOX_INFO << "http session connected: " << ep;
    }
    void do_on_http_disconnect(CyclTime /*now*/, const Endpoint& ep) noexcept override
    {
        TOOLBOX_INFO << "http session disconnected: " << ep;
    }
    void do_on_http_error(CyclTime /*now*/, const Endpoint& ep, const std::exception& e,
                          http::OStream& /*os*/) noexcept override
    {
        TOOLBOX_ERROR << "http session error: " << ep << ": " << e.what();
    }
    void do_on_http_message(CyclTime /*now*/, const Endpoint& /*ep*/, const Request& req,
                            http::OStream& os) override
    {
        const auto it = slot_map_.find(string{req.path()});
        if (it != slot_map_.end()) {
            os.reset(Status::Ok, TextPlain);
            it->second(req, os);
        } else {
            os.reset(Status::NotFound, TextPlain);
            os << "Error 404 - Page not found";
        }
        os.commit();
    }
    void do_on_http_timeout(CyclTime /*now*/, const Endpoint& ep) noexcept override
    {
        TOOLBOX_WARN << "http session timeout: " << ep;
    }

  private:
    SlotMap slot_map_;
};

} // namespace

int main()
{
    int ret = 1;
    try {

        const auto start_time = CyclTime::now();

        Reactor reactor{};
        ExampleApp app;
        app.bind("/foo", bind<on_foo>());
        app.bind("/bar", bind<on_bar>());

        const TcpEndpoint ep{TcpProtocol::v4(), 8888};
        Serv http_serv{start_time, reactor, ep, app};

        // Start service threads.
        pthread_setname_np(pthread_self(), "main");
        ReactorRunner reactor_runner{reactor, 100, "reactor"s};

        // Wait for termination.
        SigWait sig_wait;
        for (;;) {
            switch (const auto sig = sig_wait()) {
            case SIGHUP:
                TOOLBOX_INFO << "received SIGHUP";
                continue;
            case SIGINT:
                TOOLBOX_INFO << "received SIGINT";
                break;
            case SIGTERM:
                TOOLBOX_INFO << "received SIGTERM";
                break;
            default:
                TOOLBOX_INFO << "received signal: " << sig;
                continue;
            }
            break;
        }
        ret = 0;

    } catch (const std::exception& e) {
        TOOLBOX_ERROR << "exception on main thread: " << e.what();
    }
    return ret;
}
