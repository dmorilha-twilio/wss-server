#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

struct Cert {
    const char * const cert;
    const char * const key;
};

// Echoes back all received WebSocket messages
void
do_session(tcp::socket& socket, ssl::context& ctx)
{
    std::cout << "new connection was accepted" << std::endl;
    try
    {
        // Construct the websocket stream around the socket
        websocket::stream<beast::ssl_stream<tcp::socket&>> ws{socket, ctx};

        // Perform the SSL handshake
        ws.next_layer().handshake(ssl::stream_base::server);

        // Set a decorator to change the Server of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-sync-ssl");
            }));

        // Accept the websocket handshake
        ws.accept();

        for(;;)
        {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Read a message
            ws.read(buffer);

            // Echo the message back
            ws.text(ws.got_text());
            ws.write(buffer.data());
        }
    }
    catch(beast::system_error const& se)
    {
        // This indicates that the session was closed
        if(se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Echoes back all received WebSocket messages
void
wss_server(const Cert & c) {
    namespace net = boost::asio;

    try {
        const char * const address = "127.0.0.1";
        const unsigned short port = 8443;

        std::cout << "The io_context is required for all I/O" << std::endl;
        net::io_context ioc{1};

        std::cout << "The SSL context is required, and holds certificates" << std::endl;
        ssl::context ctx{ssl::context::tlsv12};

        ctx.use_certificate(net::const_buffer(c.cert, strlen(c.cert)), ssl::context::pem);

        if (nullptr != c.key) {
            ctx.use_private_key(net::const_buffer(c.key, strlen(c.key)), ssl::context::pem);
        }

        std::cout << "The acceptor receives incoming connections" << std::endl;
        tcp::acceptor acceptor{ioc, {net::ip::make_address(address), port}};

        while (true) {
            std::cout << "This will receive the new connection" << std::endl;
            tcp::socket socket{ioc};

            std::cout << "This WebSocket server is ready to accept connections at " << address << ":" << port << std::endl;
            acceptor.accept(socket);
            std::thread{std::bind(
                &do_session,
                std::move(socket),
                std::ref(ctx))}.detach();
        }

    } catch (const beast::system_error& se) {
        // This indicates that the session was closed
        if (se.code() != websocket::error::closed) {
            std::cerr << "Error: " << se.code().message() << std::endl;
        }
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(void) {
        const Cert cert{
        "-----BEGIN CERTIFICATE-----" "\n"
        "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" "\n"
        "ijNB3Q==" "\n"
        "-----END CERTIFICATE-----",

        "-----BEGIN RSA PRIVATE KEY-----" "\n"
        "MIIJJwIBAAKCAgEAuWMUdcnkYka7sgiB7xe7sY2Z4F4V12tLxCXxW9b8Ls//m5tl" "\n"
        "QgL6kPdHBxgkuu+PqIAlbar/gz6RuUQUkc/C1HPmJJ5Tq3I4xauXJcYo4CzJ480T" "\n"
        "VmX24yNCZ0RAzSS4otRqnv49GjavyqV1BtMnRl6b63iMpX5RqWRdx+4cmV+3MVGP" "\n"
        "FsH561IA1RrxxUTO8rUDz95jtvBqoh9wbv56blLyv2nRidFKOGdhX6cdW2b+CrH5" "\n"
        "Dw1J6tC8a/kBqA5OrxolgRkdINeZHnk0pMRqpjr9YUDvfpCilNWK+HFLnqhC3sUF" "\n"
        "AO1+1cDKklXyVwFMdJEN9SFLLMgXlDJUTgIQT1hehYFyXPRphDspU8aXnyWRYK2F" "\n"
        "eWHQbOFDntk7He9GgwlhGCOlfr/Itv2V3Cti+HLkFMVwVuYbgUuRcWB0t6OKD7Nf" "\n"
        "BI4PbUpj6yFDnww+Inl+PYddXAULjFH9saUNbmtlex0gMo9vzdTkWxUOMZOryFYL" "\n"
        "dyxzjFWPP5MZ2gTjAN0RVXoOqRz4M9T9JNZsR6ZXdyIlfQwf4qCQXBMoPIxNvdQv" "\n"
        "Aq/VswVVBdDnngmziFzhMc6EPJbQbjVtXfcs70YpoTIk8tGYjPnnst7CXoNv1HI8" "\n"
        "CWw6g1bf3mT8iHxKqglC75NNRPMx3KBpdMDkXkkbCB0GgPlevfwVEkjxPB8CAwEA" "\n"
        "AQKCAgBM3j5PkMT30uhkq45kuuyCdHpHQ/u+dKjXzfvD3uJ17pzHwqk7s8x9a2qy" "\n"
        "EFq6gDaK4Yejb3G+06TMsuPsib75JI1axaNfxV6w57MmYIH2Ydvcbt4DxweVqrsP" "\n"
        "4ocUCJPeQEFtZ3GieuZtGinqCIeScAwqmLOIs1Xc7NbN3XdSSRPGiVDRWODtp/Tc" "\n"
        "Z+2zpBJDJLdN66Jg6lLsoiiWfUH6mL46i//sKzGhLYx8guhzS8MuYBkrGpF5oc9e" "\n"
        "3cLIEhaiGC9Q1j7zrTG6JvujDiZCgtcpCinDyuj+2YRj+2YlhRMlF4A5oEAlFWlp" "\n"
        "P+XaGZnAA51L9OAc02KjejLmnYhxCUPvbKjQwm0DJL8BYwoKynH2Dn0rK1aeyMFK" "\n"
        "up17UJzKKbr+O2zzBzgstYCqHCa/u9eQOKaFwcYBe5BICuK8309VHr8OHeA7qs7O" "\n"
        "HBh1rUpjopNekWxKV+WjA0ts5GLwB85KprWr/tuM6GxwCx6n0TxOZgka0E9tMKWQ" "\n"
        "Ax8pMUp5wVmlzl8hmWyK97XkvJkk4fIt2nF6zyPcfTpZtBTb0jvGg7mqwLerPARN" "\n"
        "ub5n9g2a5HLAxthrD5XP+wdLRllD8ua/DJF57UrTerxM5jtnXrxqSPpvszt/BcHa" "\n"
        "E6CtPfnBJgS/1dgGGUWfF0kVc4+y5Di8SUM3aDfqf8BrGa1wQQKCAQEA2niBj141" "\n"
        "W1vKEw/xoSsH5yGsL+dgdSgJPOWxKpoN/fFaoAff7Bp7nErCdP7U56DqvlUJEHAx" "\n"
        "+cJCn9xsUd2CnioPDk6if4QRWfW3ZLUSjKvj7VIVvwVX81MxxQdx9NVxTI3oQcgs" "\n"
        "fzlCZ+/bt0wSRpbSIHCmgi3DS+pKzspYUdnm7uctENMmpfKKf1Gp+jCWH4wzHXxP" "\n"
        "d/xPvlPyWL+Jdl3QdmtCc6RjcjQDJWWEzmEgFpQ+fbBrI1rig9mOpMShxDrQCwFH" "\n"
        "Q6gREqld8OCFsy+EuQX7TiQOV1p59TqbyuvxS0BkQKr4nw6s8fObgym0lrRsrZRp" "\n"
        "LZ93n5pIAS5M7wKCAQEA2Tuub+Z4K+NM0ksa3kPbY59Zvt+Etk1KCtOUcxGtxwN4" "\n"
        "9EZvV+bSo5DCflPEigKRa8L9wyS/3460jCZWI9YosrvtJJ6cFHnlt7Gz42JF+g86" "\n"
        "+jdpcBnNpzC0dCS6pmdj4A8dPQ2S+WZ1+/MeIRT6gS11jl82GDrEiKq7+NRpci+k" "\n"
        "J0xACcW7P4N7SJXwmK/4MN3YaIb82T5QOx9ZvxhhBT9WVlIOevzMx5tjfwWnKSTW" "\n"
        "7jNtbnkFjD8UMVRxYaBVruatjIB6mM3KGY1ulh2PPGdXaqKYSSEecncONCeV2Ja1" "\n"
        "Rwugkgl+aoYZ1emywriCu7sbUBkZGe1V7XAUg9xj0QKCAQB8S286yBiHP7O6IKoS" "\n"
        "Tc8TOt8h6fvA6+RFl1Cysi8fuT8cKR7Gz1JgW1BDC/DJT43+d5bOmWZ1eIY7OUcP" "\n"
        "VH1JyxgBxc9ByTut+NQ8IaDKE8l7beYdZcbiEwUP2eqFpyHsshffxSk7wLH9PEO/" "\n"
        "BzKmoowuFKlHCw4/lK628/xHpTdz2GaVWex1IYaWZTnVs+rVTAiQz/bOhug/mrcY" "\n"
        "svBcPYeqt994t6A0i0726ckjObLt5a2gCM0LsluJNC+poo1+fQ98E2AeJ/J+sYlW" "\n"
        "0AaBKZnTQon1rSHTpRAZ6xxT2OTRMXNYL21/4POYPEbP2tx5mXDzWKp2YHHIhhhN" "\n"
        "Q7BVAoIBABvYs66ye3scww8+A260gh4OIkH+gxOfartijegGPyGAx3OAx+sbsqq2" "\n"
        "9sec01fhpmSK9V6bimEM+QXyMB6d6E+pGWgVUOqrCZB3nhiq00+Pvj6u9bwiAqPI" "\n"
        "8Yjl4GtOW5YwOc5GvC1uPfeTHayd9NK0UhpzCAspFWJziG8IQ5AYnVFQS/E0wKEO" "\n"
        "qLIQTmw0OnuBJKmZjDYI7930UdvWMzGqAdZE5IhNea6os6NnxRrYFxknzIJZzyVA" "\n"
        "PMVqDrK2C+XEukCFg7Gsx5ho8eEaXu8eTC1mNGFDdpSzwEVdQa8m+ovl6Kpvvx3G" "\n"
        "nO836utdHZ0VChZFcVtKejZapLaWfNECggEAFKWo6LAbLYd7Bbtx4iJhGfYNSID7" "\n"
        "0D3dgMcMUdKsW0H/DmBrpuDMi6yedAmBEQLT+LHai2CRMekYT6CtH39y8YO06ZPH" "\n"
        "vntXTjC0dRTZUJS/jSy/YW7Eup6ozEwr5BuIDJdNvVHCEXld2PvqGlKsguW6pgy9" "\n"
        "8TUhH+JBIiY8jm4vV9UkwKrvyqxKOcIY1tRJ0oQksmDJgHDxG8f20V5fTzqHAsjp" "\n"
        "+iRcSNE/TxkuFgsmfwOrGq/fYubbvQ4TFdUEyqip8I76J8MKVRTLF3sndBcCJJaY" "\n"
        "wW8nTavo74WQXzhCk3EVsMvfZsCmPUpFSBHdLxg41KBejjn/WmHTgfdV9g==" "\n"
        "-----END RSA PRIVATE KEY-----"};

    // Launch a websocket server into a different thread
    wss_server(cert);

    return 0;
}
