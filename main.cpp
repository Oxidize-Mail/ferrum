//
// Created by Ghostvox on 9/15/2026.
//
// fix readme
#include <dpp/appcommand.h>
#include <dpp/dpp.h>

#include <cstdlib>
#include <dotenv/dotenv.hpp>
#include <iostream>

#include "config.h"

int main() {
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
    }
  });

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "high-praise") {
      event.reply(
          "Bow to our glorious LEADER Brinhasavlin!! Creator of the "
          "mighty Potato. long may he reign!!!");
    }
  });
  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "wiz") {
      event.reply("Hey Merlin, you are a mighty wizard ");
    }
  });

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "make-a-difference") {
      event.reply(
          "If you ever wanted to make a difference in compilers of the world, "
          "then all you need to do is join up and become a SoftOmni "
          "contributor");
    }
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
      bot.guild_command_create(
          dpp::slashcommand("make-a-difference", "Make a difference",
                            bot.me.id),
          server_id);
    }
  });

  bot.start(dpp::st_wait);
}
