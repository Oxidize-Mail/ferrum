# Ferrum

A discord bot to drive engagement and foster a positive community in the Oxidize server

# Contributing

## Setup

We are using CMake to compile the project with VCPKG to handle install the required CPP libraries needed to build it.

### Prerequisites

| Tool         | Version | Notes                                                 |
| ------------ | ------- | ----------------------------------------------------- |
| clang-format | 22.0.0+ | Optional, but used to format code with `clang-format` |
| CMake        | 4.0.0+  | Required by `CMakeLists.txt`                          |
| Ninja        | any     | The `vcpkg-preset` uses the Ninja generator           |
| Compiler     | C++23   | GCC 13+, Clang 16+, or MSVC from Visual Studio 2022   |
| Git          | any     | Needed to clone VCPKG                                 |

On Linux, most of this comes from your package manager:

```sh
# Arch
sudo pacman -S cmake ninja git base-devel

# Debian/Ubuntu
sudo apt install cmake ninja-build git build-essential curl zip unzip tar pkg-config
```

### VCPKG SETUP

If you already have VCPKG installed and `VCPKG_ROOT` exported, skip to [Build](#build).

1. Clone the VCPKG repo somewhere permanent (not inside this project):

   ```sh
   git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
   ```

2. Run the bootstrap script to build the `vcpkg` executable:

   ```sh
   # Linux / macOS
   ~/vcpkg/bootstrap-vcpkg.sh

   # Windows (PowerShell)
   .\vcpkg\bootstrap-vcpkg.bat
   ```

3. Export `VCPKG_ROOT` and add VCPKG to your `PATH`. The CMake preset reads
   `VCPKG_ROOT` to find the toolchain file, so this step is not optional.

   ```sh
   # ~/.bashrc, ~/.zshrc, or your shell's rc file
   export VCPKG_ROOT="$HOME/vcpkg"
   export PATH="$VCPKG_ROOT:$PATH"
   ```

   ```powershell
   # Windows (PowerShell, persists for your user)
   [Environment]::SetEnvironmentVariable('VCPKG_ROOT', "$HOME\vcpkg", 'User')
   [Environment]::SetEnvironmentVariable('PATH', "$env:PATH;$HOME\vcpkg", 'User')
   ```

4. Open a new shell and confirm it took:

   ```sh
   echo $VCPKG_ROOT
   vcpkg --version
   ```

We use VCPKG in manifest mode, so you do **not** need to install D++ by hand.
`vcpkg.json` pins the dependency and CMake installs it into `vcpkg_installed/`
on the first configure.

### Third-party headers

`main.cpp` includes `<dotenv/dotenv.hpp>`, which is resolved from the
`third_party/` include directory (see `target_include_directories` in
`CMakeLists.txt`). The header is vendored in the repo along with its license,
so there is nothing to download or place by hand.

### Build

From the project root:

```sh
# Configure. The first run also installs D++ via VCPKG, which takes a while.
cmake --preset vcpkg-preset

# Compile
cmake --build build
```

The preset writes everything to `build/`, and the resulting binary is
`build/ferrum`. `build/` is the canonical build directory. CLion creates its own
`cmake-build-*` directories, which are gitignored and not used by CI.

On Windows the preset still asks for Ninja, so run these from a **Developer
Command Prompt for VS 2022** (or a Developer PowerShell) so that `cl.exe` and
the Windows SDK are on the path.

Note that the project builds with `-Wall -Wextra -Werror -Wpedantic`
(`/W4 /WX` under MSVC) — warnings are errors, so a warning in your patch will
fail the build.

### Configuration

The bot reads its token from a `BOT_TOKEN` environment variable, loading a
`.env` file from the current working directory at startup.

1. Copy the example file:

   ```sh
   cp .example_env .env
   ```

2. Create a Discord application at the
   [Discord Developer Portal](https://discord.com/developers/applications),
   open the **Bot** tab, and use **Reset Token** to generate a token.

3. Put that token in `.env`:

   ```
   BOT_TOKEN="your-token-here"
   ```

   `.env` is gitignored. Never commit it, and reset the token in the portal if
   it ever leaks.

4. Invite the bot to a test server from the **OAuth2 > URL Generator** tab,
   selecting the `bot` and `applications.commands` scopes.

### Run

`.env` is read relative to the working directory, so run the binary from the
project root rather than from inside `build/`:

```sh
./build/ferrum
```

You should see the shard-ready log line on startup. The bot registers its slash
commands on first run. Global commands can take up to an hour to propagate to
every server. Guild commands are registered only for the Oxidize server (the
hardcoded `server_id` in `main.cpp`) and appear immediately.

Currently registered commands:

| Command   | Scope  | Description  |
| --------- | ------ | ------------ |
| `/ping`   | Global | Ping pong!   |
| `/praise` | Global | Get potatoes |
| `/high-praise` | Guild  | give high praise |
| `/wiz`    | Guild  | wizard       |
