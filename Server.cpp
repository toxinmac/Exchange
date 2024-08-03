#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "json.hpp"
#include "Common.hpp"

using boost::asio::ip::tcp;
struct Balance{
    int rub;
    int usd;
    Balance(): rub(0),usd(0){};
    Balance(const int r, const int u) : rub(r), usd(u){}
};
struct Order{
    std::string userId;
    int volume;
    int volume_s;
    int price;
    std::string orderId;
    Order(const std::string& user, double vol, double pr, const std::string& id) : userId(user), volume(vol),volume_s(vol), price(pr), orderId(id){}
};
struct Order_done{
    std::string userId_Sell;
    std::string userId_Buy;
    int volume;
    int price;
    Order_done(const std::string& user_a,const std::string& user_b, double vol, double pr) : userId_Sell(user_a),userId_Buy(user_b), volume(vol),price(pr){}
};
class Core
{
public:
     // Возвращает баланс пользовтаеля по заданной ID     
    std::string check_balance(const std::string& aUserId){
         const auto userIt = mUsers_balance.find(std::stoi(aUserId));
         std::string res = " ";
         if (userIt == mUsers_balance.cend())
        {
            res = "Error! Unknown User";
        }else{
        res += std::to_string(userIt->second.rub) + " RUB " + std::to_string(userIt->second.usd) + " USD";
        }
        return res;
    }
    // Изменяет баланс заданного пользователя на заданную величину
    void Add_Balance(const std::string& aUserId, const int& rub, const int& usd){
        const auto userIt = mUsers_balance.find(std::stoi(aUserId));
            userIt->second.rub+=rub;
            userIt->second.usd+=usd;
    }
    // "Регистрирует" нового пользователя и возвращает его ID.
    std::string RegisterNewUser(const std::string& aUserName)
    {
        size_t newUserId = mUsers.size();
        mUsers[newUserId] = aUserName;
        Balance balance(0,0);
        mUsers_balance[newUserId] = balance;

        return std::to_string(newUserId);
    }

    // Запрос имени клиента по ID
    std::string GetUserName(const std::string& aUserId)
    {
        const auto userIt = mUsers.find(std::stoi(aUserId));
        if (userIt == mUsers.cend())
        {
            return "Error! Unknown User";
        }
        else
        {
            return userIt->second;
        }
    }
    // Добавляет заявку на продажу
    std::string AddSellOrder(const std::string& userId, double volume, double price){
        std::string orderId = std::to_string(mSellOrders.size());
        Order order(userId, volume, price, orderId);
        mSellOrders.push_back(order);
        if(!mBuyOrders.empty())
        CheckForTrades();
        return orderId;
    }
    // Добавляет заявку на покупку
    std::string AddBuyOrder(const std::string& userId, double volume, double price){
        std::string orderId = std::to_string(mBuyOrders.size());
        Order order(userId, volume, price, orderId);
        mBuyOrders.push_back(order);
        if(!mSellOrders.empty())
        CheckForTrades();
        return orderId;
    }
    std::string Completed_transactions(const std::string& auserId){
        std::string res = "Sales: \n";
        int k = 0;
        for(size_t i = 0; i < DoneTrade.size(); i++){
            if(DoneTrade[i].userId_Sell == auserId ){
                res +=std::to_string(++k) + ": " + std::to_string(DoneTrade[i].volume) + " USD for " + std::to_string(DoneTrade[i].price) + " RUB. Buyer - Name: " + GetUserName(DoneTrade[i].userId_Buy) + ". ID - " + DoneTrade[i].userId_Buy + "\n" ;
            }
        }
        if(res == "Sales: \n") res += "No deals found\n";

        std::string res1 = "Purchases: \n";
        k = 0;
        for(size_t i = 0; i < DoneTrade.size(); i++){
            if(DoneTrade[i].userId_Buy == auserId ){
                res1 +=std::to_string(++k) + ": " + std::to_string(DoneTrade[i].volume) + " USD for " + std::to_string(DoneTrade[i].price) + " RUB. Seller - Name: " + GetUserName(DoneTrade[i].userId_Sell) + ". ID - " + DoneTrade[i].userId_Sell + "\n" ;
            }
        }
        if(res1 == "Purchases: \n") res1 += "No purchases found\n";
        return res + res1;
    }
    std::string active_application(const std::string& auserId){
        std::string res = "Sales applications: \n";
        int k = 0;
        for(size_t i = 0; i < mSellOrders.size(); i++){
            if(mSellOrders[i].userId == auserId ){
                res +=std::to_string(++k) + ": " + std::to_string(mSellOrders[i].volume) + " / " + std::to_string(mSellOrders[i].volume_s) + " USD for " + std::to_string(mSellOrders[i].price) + " RUB\n";
            }
        }
        if(res == "Sales applications: \n") res += "No applications found\n";
        k = 0;
        std::string res1= "Purchase requests:\n";
        for(size_t i = 0; i < mBuyOrders.size(); i++){
            if(mBuyOrders[i].userId == auserId ){
                res1 +=std::to_string(++k) + ": " + std::to_string(mBuyOrders[i].volume) + " / " + std::to_string(mBuyOrders[i].volume_s) + " USD for " + std::to_string(mBuyOrders[i].price) + " RUB\n";
            }
        }
        if(res1 == "Purchase requests:\n") res1 += "No applications found\n";
        return res+res1;
    }
    void quotes(const int& price){
        if(mas[0] == 0){
            mas[0] = price; //start
            mas[1] = price; //min
            mas[2] = price; //max
            mas[3] = price; //final
            mas[4] = price; //Sum
        }else{
            if(price < mas[1]) mas[1] = price;//min
            if(price > mas[2]) mas[2] = price;//max
            mas[3] = price; //final
            mas[4] +=price; //Sum
        }
    }
    std::string show_q(){
        std::string res;
        res += "\nOpening price: " + std::to_string(mas[0]) + "\nHighest and lowest prices: " + std::to_string(mas[2]) + " " + std::to_string(mas[1]) + "\nClosing price:" + std::to_string(mas[3]) + "\nTrading volume: " + std::to_string(mas[4]);
        return res;
    }



private:
    // <UserId, UserName>
    std::map<size_t, std::string> mUsers;
    std::map<size_t, Balance> mUsers_balance;
    std::vector<Order> mSellOrders;
    std::vector<Order> mBuyOrders;
    std::vector<Order> DoneApp;
    std::vector<Order_done> DoneTrade;
    int mas[5];
    
auto max_b(){
    int max_price = 0;
        auto buy = mBuyOrders.end();
        for (auto buyIt = mBuyOrders.begin(); buyIt != mBuyOrders.end(); buyIt++)
            {
                if(buyIt->price > max_price){
                    max_price = buyIt->price;
                    buy = buyIt;
                } 
            }
            return buy;
}
auto min_s(const std::string& temp){
    int min_price = 1000000000;
        auto sel = mSellOrders.end();
        for (auto sellIt = mSellOrders.begin(); sellIt != mSellOrders.end(); sellIt++)
            {
                if(sellIt->price < min_price && sellIt->userId != temp){
                    min_price = sellIt->price;
                    sel = sellIt;
                } 
            }
            return sel;
}
    void CheckForTrades()
    {
            for (auto sellIt = mSellOrders.begin(); sellIt != mSellOrders.end();)
        {
            auto buy = max_b();
            auto sel = min_s(buy->userId);
            if(buy == mBuyOrders.end() || sel == mSellOrders.end() ) sellIt = mSellOrders.end();
            else if(sel->price <= buy->price ){
                double tradeVolume = std::min(sel->volume, buy->volume);

                    // Обновление объемов заявок
                    sel->volume -= tradeVolume;
                    Add_Balance(sel->userId, tradeVolume * buy->price,-tradeVolume);
                    buy->volume -= tradeVolume;
                    Add_Balance(buy->userId, -tradeVolume * buy->price,+tradeVolume);
                    
                    Order_done o(sel->userId, buy->userId, tradeVolume, buy->price);
                    DoneTrade.push_back(o);

                    // Отправка сообщения о сделке
                    std::string message = "Trade executed: " + GetUserName(sel->userId) + " sold " +
                                          std::to_string(tradeVolume) + " @ " + std::to_string(buy->price) +
                                          " to " + GetUserName(buy->userId) + ".\n";
                    std::cout << message;

                    // Если все объемы заявок равны нулю - удаляем их
                    if (buy->volume == 0)
                    {
                        DoneApp.push_back(*buy);
                        mBuyOrders.erase(buy);
                    }
                    if (sel->volume == 0)
                    {
                        DoneApp.push_back(*sel);
                        sellIt = mSellOrders.erase(sel);
                        sellIt = mSellOrders.begin();
                    }
            }else sellIt = mSellOrders.end();
            }
    }


};

Core& GetCore()
{
    static Core core;
    return core;
}

class session
{
public:
    session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            data_[bytes_transferred] = '\0';

            // Парсим json, который пришёл нам в сообщении.
            auto j = nlohmann::json::parse(data_);
            auto reqType = j["ReqType"];

            std::string reply = "Error! Unknown request type";
            if (reqType == Requests::Registration)
            {
                // Это реквест на регистрацию пользователя.
                // Добавляем нового пользователя и возвращаем его ID.
                reply = GetCore().RegisterNewUser(j["Message"]);
            }
            else if (reqType == Requests::Hello)
            {
                // Это реквест на приветствие.
                // Находим имя пользователя по ID и приветствуем его по имени.
                reply = "Hello, " + GetCore().GetUserName(j["UserId"]) + "!\n";
            }
            else if(reqType == Requests::Trade_r)
            { 
                std::string userId = j["UserId"];
                int volume = j["Volume"];
                int price = j["Price"];
                GetCore().quotes(j["Price"]);
                reply = "Sell order added: " + GetCore().AddSellOrder(userId, volume, price) + "!\n";
            
            }else if(reqType == Requests::Buy_r)
            { 
                std::string userId = j["UserId"];
                int volume = j["Volume"];
                int price = j["Price"];
                GetCore().quotes(j["Price"]);
                reply = "Buy order added: " + GetCore().AddBuyOrder(userId, volume, price) + "!\n";
            
            }else if(reqType == Requests::Check_balance)
            {
                reply = "Your balance: " + GetCore().check_balance(j["UserId"]) + "\n";   
            }else if(reqType == Requests::Check_application)
            {
                reply = "Your active requests:\n" + GetCore().active_application(j["UserId"]) + "\n";
            }else if(reqType == Requests::Completed_transactions)
            {
                reply = "Your completed transactions:\n" + GetCore().Completed_transactions(j["UserId"]) + "\n";
            }else if(reqType == Requests::show_quotes)
            {
                reply = "Quote history:\n" + GetCore().show_q() + "\n";
            }

            boost::asio::async_write(socket_,
                boost::asio::buffer(reply, reply.size()),
                boost::bind(&session::handle_write, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_service& io_service)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "Server started! Listen " << port << " port" << std::endl;

        session* new_session = new session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    }

    void handle_accept(session* new_session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
            new_session = new session(io_service_);
            acceptor_.async_accept(new_session->socket(),
                boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete new_session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        static Core core;

        server s(io_service);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}