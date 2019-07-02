#include <iostream>
#include <functional>
#include <memory>
#include <restbed>
#include <string>

class ErrorHandler
{
public:
    void notFound(const std::shared_ptr< restbed::Session > _session)
    {
        std::string type = "text/plain";
        std::string body = "Unknown request: \""
                           + _session->get_request()->get_path()
                           + "\"\n";
        const std::multimap< std::string, std::string > headers
        {
            { "Content-Type", type },
            { "Content-Length", std::to_string(body.length()) }
        };
        _session->close(restbed::OK, body, headers);
    }
};

class HealthHandler
{
public:
    void handle(const std::shared_ptr< restbed::Session > _session)
    {
        std::string type = "application/json";
        std::string body = "{\"status\":\"UP\"}\n";
        const std::multimap< std::string, std::string > headers
        {
            { "Content-Type", type },
            { "Content-Length", std::to_string(body.length()) }
        };
        _session->close(restbed::OK, body, headers);
    }
};

class Server
{
public:
    void notFound(
        const std::function <void (
            const std::shared_ptr< restbed::Session >)
        > & _handler)
    {
        m_service.set_not_found_handler(_handler);
    }

    void route(
        const std::string & _path,
        const std::string & _method,
        const std::function <void (
            const std::shared_ptr< restbed::Session >)
        > & _handler)
    {
        auto resource = std::make_shared<restbed::Resource>();
        resource->set_path(_path);
        resource->set_method_handler(_method, _handler);
        m_service.publish(resource);
    }

    void startAndWait(
        const std::string & _ip,
        unsigned int _port,
        unsigned int _threads)
    {
        m_service.set_ready_handler(std::bind(
                                        &Server::handle_startup_message,
                                        this,
                                        std::placeholders::_1));

        auto settings = std::make_shared<restbed::Settings>();
        settings->set_bind_address(_ip);
        settings->set_port(_port);
        settings->set_default_header("Connection", "close");
        settings->set_worker_limit(_threads);
        m_service.start(settings);
        m_service.stop();
    }

private:
    void handle_startup_message(restbed::Service &)
    {
        std::cout << "service fired up..." << std::endl;
    }

    restbed::Service m_service;
};

int main(const int /*_argc*/, const char ** /*_argv*/)
{
    ErrorHandler eh;
    HealthHandler hh;

    Server server;
    server.notFound(
        std::bind(&ErrorHandler::notFound, eh, std::placeholders::_1));
    server.route(
        "/health", "GET",
        std::bind(&HealthHandler::handle, hh, std::placeholders::_1));
    server.startAndWait("127.0.0.1", 8080, 2);

    return EXIT_SUCCESS;
}
