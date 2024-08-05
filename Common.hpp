#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

static short port = 5555;

namespace Requests {
static std::string Registration = "Reg";
static std::string Hello = "Hel";
static std::string Trade_r = "Trade_r";
static std::string Buy_r = "Buy_r";
static std::string Check_balance = "Check_balance";
static std::string Check_application = "Check_application";
static std::string Completed_transactions = "Completed_transactions";
static std::string show_quotes = "show_quotes";
}  // namespace Requests

#endif  // CLIENSERVERECN_COMMON_HPP