#include <gtest/gtest.h>

#include "../Server.h"

class CoreTest : public ::testing::Test {
 protected:
  Core core;
};
TEST_F(CoreTest, RegisterNewUser) {
  std::string userId1 = core.RegisterNewUser("Alice");
  ASSERT_EQ(std::stoi(userId1),
            0);  // Проверяем, что первый пользователь получил ID 0

  std::string userId2 = core.RegisterNewUser("Bob");
  ASSERT_EQ(std::stoi(userId2),
            1);  // Проверяем, что второй пользователь получил ID 1
}
TEST_F(CoreTest, CheckBalance) {
  std::string userId = core.RegisterNewUser("Alice");

  // Изначально баланс должен быть нулевым
  std::string balance = core.check_balance(userId);
  EXPECT_EQ(balance, "0 RUB 0 USD");

  // Добавим немного баланса и проверим снова
  core.Add_Balance(userId, 100, 50);  // Обновляем баланс
  balance = core.check_balance(userId);
  EXPECT_EQ(balance, "100 RUB 50 USD");

  balance = core.check_balance(std::to_string(10));
  EXPECT_EQ(balance, "Error! Unknown User");
}
TEST_F(CoreTest, GetUserName) {
  std::string name = core.GetUserName("10");
  EXPECT_EQ(name, "Error! Unknown User");

  std::string userId1 = core.RegisterNewUser("Endy");
  ASSERT_EQ(std::stoi(userId1), 0);

  std::string name_endy = core.GetUserName("0");
  EXPECT_EQ(name_endy, "Endy");
}
TEST_F(CoreTest, AddSellOrder) {
  std::string res = core.AddSellOrder("26", 10, 62);
  EXPECT_EQ(res, "0");
  std::string res1 = core.AddSellOrder("27", 10, 62);
  EXPECT_EQ(res1, "1");
}
TEST_F(CoreTest, AddBuyOrder) {
  std::string res = core.AddBuyOrder("26", 10, 62);
  EXPECT_EQ(res, "0");
  std::string res1 = core.AddBuyOrder("27", 10, 62);
  EXPECT_EQ(res1, "1");
}
TEST_F(CoreTest, Completed_transactions) {
  std::string userId1 = core.RegisterNewUser("Alice");
  std::string nodeals = core.Completed_transactions(userId1);
  EXPECT_EQ(nodeals,
            "Sales: \nNo deals found\nPurchases: \nNo purchases found\n");
  std::string userId2 = core.RegisterNewUser("Anton");
  ASSERT_EQ(std::stoi(userId2),
            1);  // Проверяем, что второй пользователь получил ID 1
  std::string userId3 = core.RegisterNewUser("Endy");
  ASSERT_EQ(std::stoi(userId3),
            2);  // Проверяем, что третий пользователь получил ID 2

  std::string res = core.AddBuyOrder("0", 10, 62);
  EXPECT_EQ(res, "0");
  std::string res1 = core.AddBuyOrder("1", 20, 63);
  EXPECT_EQ(res1, "1");
  std::string res2 = core.AddSellOrder("2", 50, 61);
  EXPECT_EQ(res2, "0");

  std::string ans = core.Completed_transactions("0");
  EXPECT_EQ(ans,
            "Sales: \nNo deals found\nPurchases: \n1: 10 USD for 62 RUB. "
            "Seller - Name: Endy. ID - 2\n");

  std::string res3 = core.AddBuyOrder("0", 10, 62);
  EXPECT_EQ(res3, "0");

  std::string ans1 = core.Completed_transactions("0");
  EXPECT_EQ(ans1,
            "Sales: \nNo deals found\nPurchases: \n1: 10 USD for 62 RUB. "
            "Seller - Name: Endy. ID - 2\n2: 10 USD for 62 RUB. Seller - Name: "
            "Endy. ID - 2\n");

  std::string res4 = core.AddSellOrder("0", 10, 60);
  EXPECT_EQ(res4, "1");

  std::string res5 = core.AddBuyOrder("1", 10, 62);
  EXPECT_EQ(res5, "0");

  std::string ans2 = core.Completed_transactions("0");
  EXPECT_EQ(ans2,
            "Sales: \n1: 10 USD for 62 RUB. Buyer - Name: Anton. ID - "
            "1\nPurchases: \n1: 10 USD for 62 RUB. Seller - Name: Endy. ID - "
            "2\n2: 10 USD for 62 RUB. Seller - Name: Endy. ID - 2\n");
}
TEST_F(CoreTest, Active_application) {
  core.RegisterNewUser("Alice");
  core.RegisterNewUser("Anton");
  core.RegisterNewUser("Endy");

  std::string res = core.active_application("0");
  EXPECT_EQ(res,
            "Sales applications: \nNo applications found\nPurchase "
            "requests:\nNo applications found\n");

  core.AddBuyOrder("0", 10, 62);
  core.AddBuyOrder("0", 10, 72);
  core.AddBuyOrder("0", 10, 82);

  core.AddSellOrder("0", 10, 82);
  core.AddSellOrder("0", 10, 92);

  std::string res1 = core.active_application("0");
  EXPECT_EQ(res1,
            "Sales applications: \n1: 10 / 10 USD for 82 RUB\n2: 10 / 10 USD "
            "for 92 RUB\nPurchase requests:\n1: 10 / 10 USD for 62 RUB\n2: 10 "
            "/ 10 USD for 72 RUB\n3: 10 / 10 USD for 82 RUB\n");
}
TEST_F(CoreTest, quotes) {
  core.RegisterNewUser("Alice");
  core.RegisterNewUser("Anton");
  core.RegisterNewUser("Endy");
  std::string res = core.show_q();
  EXPECT_EQ(res,
            "\nOpening price: 0\nHighest and lowest prices: 0 0\nClosing "
            "price: 0\nTrading volume: 0");

  core.AddBuyOrder("0", 10, 62);
  core.AddBuyOrder("1", 20, 72);

  core.AddSellOrder("2", 50, 41);

  core.AddBuyOrder("0", 10, 62);
  core.AddBuyOrder("1", 10, 72);

  core.AddSellOrder("2", 30, 50);

  core.AddBuyOrder("0", 10, 55);
  core.AddBuyOrder("1", 20, 51);

  std::string res1 = core.show_q();
  EXPECT_EQ(res1,
            "\nOpening price: 72\nHighest and lowest prices: 72 51\nClosing "
            "price: 51\nTrading volume: 80");
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}