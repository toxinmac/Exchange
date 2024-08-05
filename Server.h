#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <cstdlib>
#include <iostream>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;
struct Balance {
  int rub;
  int usd;
  Balance() : rub(0), usd(0){};
  Balance(const int r, const int u) : rub(r), usd(u) {}
};
struct Order {
  std::string userId;
  int volume;
  int volume_s;
  int price;
  std::string orderId;
  Order(const std::string& user, int vol, int pr, const std::string& id)
      : userId(user), volume(vol), volume_s(vol), price(pr), orderId(id) {}
};
struct Order_done {
  std::string userId_Sell;
  std::string userId_Buy;
  int volume;
  int price;
  Order_done(const std::string& user_a, const std::string& user_b, int vol,
             int pr)
      : userId_Sell(user_a), userId_Buy(user_b), volume(vol), price(pr) {}
};
class Core {
 public:
  // Возвращает баланс пользовтаеля по заданной ID
  std::string check_balance(const std::string& aUserId);
  // Изменяет баланс заданного пользователя на заданную величину
  void Add_Balance(const std::string& aUserId, const int& rub, const int& usd);
  // "Регистрирует" нового пользователя и возвращает его ID.
  std::string RegisterNewUser(const std::string& aUserName);
  // Запрос имени клиента по ID
  std::string GetUserName(const std::string& aUserId);
  // Добавляет заявку на продажу
  std::string AddSellOrder(const std::string& userId, const int& volume,
                           const int& price);
  // Добавляет заявку на покупку
  std::string AddBuyOrder(const std::string& userId, const int& volume,
                          const int& price);
  std::string Completed_transactions(const std::string& auserId);
  std::string active_application(const std::string& auserId);
  void quotes(const int& vol, const int& price);
  std::string show_q();

 private:
  // <UserId, UserName>
  std::map<size_t, std::string> mUsers;
  std::map<size_t, Balance> mUsers_balance;
  std::vector<Order> mSellOrders;
  std::vector<Order> mBuyOrders;
  std::vector<Order> DoneApp;
  std::vector<Order_done> DoneTrade;
  int mas[5];

  auto max_b();
  auto min_s(const std::string& temp);
  void CheckForTrades();
};

Core& GetCore();

class session {
 public:
  session(boost::asio::io_service& io_service);
  tcp::socket& socket();
  void start();
  // Обработка полученного сообщения.
  void handle_read(const boost::system::error_code& error,
                   size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

 private:
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class server {
 public:
  server(boost::asio::io_service& io_service);
  void handle_accept(session* new_session,
                     const boost::system::error_code& error);

 private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};
