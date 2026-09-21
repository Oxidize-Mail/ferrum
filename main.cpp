//
// Created by Ghostvox on 9/15/2026.
//
// fix readme
#include <dpp/appcommand.h>
#include <dpp/dpp.h>

#include <dotenv/dotenv.hpp>
#include <iostream>
#include <optional>

#include "commands.h"
#include "config.h"

int main() {
  dotenv(".env");
  const std::string token = get_bot_token();

  if (token.empty()) {
    std::cerr << "BOT_TOKEN environment variable is not set." << std::endl;
    return 1;
  }
  commands::CommandRegistry registry;

  registry.register_command({.name = "ping",
                             .description = "ping JKBoyo",
                             .guild_id = std::nullopt,
                             .handler = commands::ping});

  registry.register_command({.name = "praise",
                             .description = "Get potatoes",
                             .guild_id = std::nullopt,
                             .handler = commands::praise});

  // Guild-specific commands
  registry.register_command({.name = "wiz",
                             .description = "wizard",
                             .guild_id = server_id,
                             .handler = commands::wiz});

  registry.register_command({.name = "high-praise",
                             .description = "give high praise",
                             .guild_id = server_id,
                             .handler = commands::high_praise});
  registry.register_command({.name = "make-a-difference",
                             .description = "make a difference",
                             .guild_id = server_id,
                             .handler = commands::make_a_difference});

  dpp::cluster bot(token);

  bot.on_log(dpp::utility::cout_logger());

  bot.on_slashcommand([&registry](const dpp::slashcommand_t& event) {
    if (const auto* handler = registry.find(event.command.get_command_name())) {
      event.reply((*handler)());
    }
  });

  bot.on_ready([&bot, registry](const dpp::ready_t& event) {
    std::cout << "Event: " << event.shard_id << " is ready." << std::endl;
    const std::vector<commands::Command>& cmds = registry.get_all_commands();
    for (auto c : cmds) {
      if (c.guild_id == std::nullopt) {
        bot.global_command_create(
            dpp::slashcommand(c.name, c.description, bot.me.id));
      } else {
        bot.guild_command_create(
            dpp::slashcommand(c.name, c.description, bot.me.id),
            c.guild_id.value());
      }
    }
  });

  bot.start(dpp::st_wait);
}
