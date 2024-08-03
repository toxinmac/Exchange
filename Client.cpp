#include <iostream>
#include <boost/asio.hpp>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

// Отправка сообщения на сервер по шаблону.
void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aRequestType,
    const std::string& aMessage,
    const int& Volume_,
    const int& Price_)
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;
    req["Volume"] = Volume_;
    req["Price"] = Price_;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    // Для регистрации Id не нужен, заполним его нулём
    SendMessage(aSocket, "0", Requests::Registration, name,0,0);
    return ReadMessage(aSocket);
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        // Мы предполагаем, что для идентификации пользователя будет использоваться ID.
        // Тут мы "регистрируем" пользователя - отправляем на сервер имя, а сервер возвращает нам ID.
        // Этот ID далее используется при отправке запросов.
        std::string my_id = ProcessRegistration(s);

        while (true)
        {
            // Тут реализовано "бесконечное" меню.
            std::cout << "Menu:\n"
                         "1) Hello Request\n"
                         "2) Exit\n"
                         "3) Trade\n"
                         "4) Buy\n"
                         "5) Check balance\n"
                         "6) Active applications \n"
                         "7) Completed transactions \n"
                         "8) Quote history \n"
                         << std::endl;

            short menu_option_num;
            std::cin >> menu_option_num;
            switch (menu_option_num)
            {
                case 1:
                {
                    // Для примера того, как может выглядить взаимодействие с сервером
                    // реализован один единственный метод - Hello.
                    // Этот метод получает от сервера приветствие с именем клиента,
                    // отправляя серверу id, полученный при регистрации.
                    SendMessage(s, my_id, Requests::Hello, "",0,0);
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2:
                {
                    exit(0);
                    break;
                }
                case 3:{
                    std::cout << "Enter sales volume and price per piece: ";
                    int volume, price;
                    std::cin >> volume >> price;
                   
                    SendMessage(s, my_id, Requests::Trade_r, "",volume, price);
                    std::cout <<"\nTry\n";
                    std::cout << ReadMessage(s);
                }

                break;
                case 4:{
                    std::cout << "Enter the purchase volume and price per piece: ";
                    int volume, price;
                    std::cin >> volume >> price;
                   
                    SendMessage(s, my_id, Requests::Buy_r, "",volume, price);
                    std::cout << ReadMessage(s);

                }
                break;
                case 5:{

                    SendMessage(s, my_id, Requests::Check_balance, "",0, 0);
                    std::cout << ReadMessage(s);

                }
                break;
                case 6:{

                    SendMessage(s, my_id, Requests::Check_application, "",0, 0);
                    std::cout << ReadMessage(s);

                }
                break;
                case 7:{

                    SendMessage(s, my_id, Requests::Completed_transactions, "",0, 0);
                    std::cout << ReadMessage(s);

                }
                break;
                case 8:{

                    SendMessage(s, my_id, Requests::show_quotes, "",0, 0);
                    std::cout << ReadMessage(s);

                }
                break;
                default:
                {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}