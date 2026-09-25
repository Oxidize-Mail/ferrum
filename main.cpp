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
  const auto config_path = config::default_path();
  if (!config_path) {
    std::cerr << config_path.error().message << "\n";
    return 1;
  }

  auto cfg = config::get_config(*config_path);
  if (!cfg) {
    std::cerr << cfg.error().message << "\n";
    return 1;
  }

  long long server_id = cfg->get_server_id();
  std::string bot_token = cfg->get_token();

  // A freshly written config carries placeholders; starting the bot with one
  // would only fail at the Discord handshake, a long way from the actual cause.
  if (bot_token.empty() || bot_token == "0") {
    std::cerr << "Bot token is not set. Edit " << *config_path
              << " and set bot.token\n";
    return 1;
  }

  if (server_id == 0) {
    std::cerr << "ServerID is not set. Edit " << *config_path
              << " and set server.id\n";
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

  dpp::cluster bot(bot_token);

  bot.on_log(dpp::utility::cout_logger());

  bot.on_slashcommand([&registry](const dpp::slashcommand_t& event) {
    if (const auto* handler = registry.find(event.command.get_command_name())) {
      event.reply((*handler)());
    }
  });

  bot.on_ready([&bot, registry](const dpp::ready_t& event) {
    std::cout << "Event: " << event.shard_id << " is ready." << std::endl;
    if (dpp::run_once<struct register_bot_commands>()) {
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
    }
  });

  bot.start(dpp::st_wait);
}
