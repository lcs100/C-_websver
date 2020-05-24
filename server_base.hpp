#ifndef SERVER_BASE_HPP
#define SERVER_BASE_HPP


#include <boost/asio.hpp>

#include <regex>
#include <unordered_map>
#include <thread>

namespace ShiyanlouWeb {

    template<typename socket_type>
    class ServerBase {
        public:
            resource_type resource;
            resource_type default_resource;

            ServerBase(unsigned short port, size_t num_threads=1) :
                endpoint(boost::asio::ip::tcp::v4(), port),
                acceptor(m_io_service, endpoint),
                num_threads(num_threads) {}

            void start();
        
        protected:
            boost::asio::io_service m_io_service;
            boost::asio::ip::tcp::endpoint endpoint;
            boost::asio::tcp::acceptor acceptor;
            size_t num_threads;
            std::vector<std::thread> threads;

            std::vector<resource_type::iterator> all_resources;

            virtual void accept() {}
            void process_request_and_respond(std::shared_ptr<socket_type> socket) const;
        
    };

    template<typename socket_type>
    class Server : public ServerBase<socket_type> {};

    struct Request {
        std::string method, path, http_version;
        std::shared_ptr<std::istream> content;
        
        std::unordered_map<std::string, std::string> headr;

        std::smatch path_match;
    };

    typedef std::map<std::string, std::unordered_map<std::string,
        std::function<void(std::ostram&, Request&)>>> resource_type;

    void ServerBase::start(){
        for(auto it = resource.begin(); it != resource.end();it++)
            all_resources.push_back(it);

        for(auto it = default_resource.begin(); it != default_resource.end(); it++)
            all_resources.push_back(it);

        accept();

        for(size_t c = 1; c < num_threads;c++){
            threads.emplace_back([this](){
                m_io_service.run();
            });
        }

        m_io_service.run();

        for(auto& t : threads)
            t.join();
    }

    void ServerBase::process_request_and_respond(std::shared_ptr<socket_type> socket) const {
        auto read_buffer = std::make_shared<boost::asio::streambuf>();

        boost::asio::async_read_until(*socket, *read_buffer, "\r\n\r\n",
            [this, socket, read_buffer](const boost::system::error_code& ec, size_t bytes_transferred){
                
            }
            )
    }



}


#endif