//
// Created by Ghostvox on 9/15/2026.
//
//fix readme
#include <dpp/appcommand.h>
#include <dpp/dpp.h>

#include <cstdlib>
#include <dotenv/dotenv.hpp>
#include <iostream>

namespace {
std::string get_bot_token() {
#ifdef _MSC_VER
  char* value = nullptr;
  size_t len = 0;
  _dupenv_s(&value, &len, "BOT_TOKEN");
  std::unique_ptr<char, decltype(&free)> guard(value, free);
  return value ? std::string(value) : std::string();
#else
  const char* value = std::getenv("BOT_TOKEN");
  return value ? std::string(value) : std::string();
#endif
}
}  // namespace

int main() {
  constexpr long long server_id = 1'425'936'348'438'859'836;
  dotenv(".env");
  const std::string token = get_bot_token();
  if (token.empty()) {
    std::cerr << "BOT_TOKEN environment variable is not set." << std::endl;
    return 1;
  }

  // this is a useless comment

  dpp::cluster bot(token);

  bot.on_log(dpp::utility::cout_logger());

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "ping") {
      event.reply("JKBoyo shush your dirty mouth!");
    }
  });

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "praise") {
      event.reply("Bow to our glorious LEADER Brinhasavlin");
    };
  });

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "high-praise") {
      event.reply(
          "Bow to our glorious LEADER Brinhasavlin!! Creator of the "
          "mighty Potato. long may he reign!!!");
    };
  });
  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "wiz") {
      event.reply("Hey Merlin, you are a mighty wizard ");
    };
  });

  bot.on_ready([&bot](const dpp::ready_t& event) {
    std::cout << "Event: " << event.shard_id << " is ready." << std::endl;
    if (dpp::run_once<struct register_bot_commands>()) {
      bot.global_command_create(
          dpp::slashcommand("ping", "Ping pong!", bot.me.id));

      bot.global_command_create(
          dpp::slashcommand("praise", "Get potatoes", bot.me.id));

      bot.guild_command_create(dpp::slashcommand("wiz", "wizard", bot.me.id),
                               server_id);
      bot.guild_command_create(
          dpp::slashcommand("high-praise", "give high praise", bot.me.id),
          server_id);
    }
  });

  bot.start(dpp::st_wait);
}
