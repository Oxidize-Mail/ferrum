//
// Created by ghostvox on 9/17/26.
//
#include <commands.h>

#include "config.h"
#include "gtest/gtest.h"


TEST(Registry, TestFerrum)
{
    commands::CommandRegistry registry;

    registry.register_command({
        .name = "ping",
        .description = "ping JKBoyo",
        .guild_id = std::nullopt,
        .handler = commands::ping
    });

    registry.register_command({
        .name = "praise",
        .description = "Get potatoes",
        .guild_id = 123,
        .handler = commands::praise
    });

    const std::function<std::string()>* handler = registry.find("ping");
    std::string response = (*handler)();
    EXPECT_EQ(response, "JKBoyo shush your dirty mouth!");

    const std::function<std::string()>* fakeHandler = registry.find("faker");
    EXPECT_EQ(fakeHandler, nullptr);

    std::vector<commands::Command> cmds = registry.get_all_commands();
    EXPECT_EQ(cmds.size(), 2);
}
