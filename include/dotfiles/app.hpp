//       _       _    __ _ _
//      | |     | |  / _(_) |
//    __| | ___ | |_| |_ _| | ___  ___
//   / _` |/ _ \| __|  _| | |/ _ \/ __|
//  | (_| | (_) | |_| | | | |  __/\__ \
//   \__,_|\___/ \__|_| |_|_|\___||___/
//  https://github.com/risingstarfish/dotfiles-cpp
//  version 0.1.0
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Pierce Katai <169690632+risingstarfish@users.noreply.github.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once


#include "dotfiles/actions.hpp"

#include "reflex/cli.hpp"


namespace rs_dotfiles {
using namespace reflex;


struct[[= cli::command {"Dotfiles installer for Mac, Linux, Windows, and WSL"}]] dotfiles {
    //===============================================================================================================
    // base options
    //===============================================================================================================
    struct common_options {
        // behaviour
        static constexpr cli::section section5 = {"BEHAVIOUR"};
        [[= cli::option {"-n,--dry-run", "Print planned actions without modifying the disk."}.flag()]] bool dry_run =
          false;
        [[= cli::option {"-K,--autorestart", "Automatically restart shell at script end."}.flag()]] bool autorestart =
          false;
        [[= cli::option {"--no-regen", "Do not regenerate relevant files if they already exist."}
            .flag()]] bool                                                                        no_regen    = false;
        [[= cli::option {"--no-backup", "Do not backup pre-existing copied files."}.flag()]] bool no_backup   = false;
        [[= cli::option {"-U,--interactive", "Prompt for confirmation before every action/modification."}
            .flag()]] bool                                                                        interactive = false;
        [[= cli::option {"--noconfirm", "Do not prompt for any confirmation."}.flag()]] bool      noconfirm   = false;
        [[= cli::option {"-y,--yes", "Auto-accept yes to prompts. Alias to --noconfirm."}.flag()]] bool yes   = false;
        [[= cli::option {"--force", "Overwrite existing files/links without backup."}.flag()]] bool     force = false;

        // logging
        static constexpr cli::section section6 = {"LOGGING"};
        [[= cli::option {"-d,--debug", "Print debug output (useful for diagnosing)."}.flag()]] bool     debug = false;
        [[= cli::option {"-q,--quiet", "Suppress all standard output except errors."}.flag()]] bool     quiet = false;
        /* FIXME: enum*/ [[= cli::option {"--log-level <level>",
                                          "Set log verbosity <level>. Valid options are: trace, debug, info, "
                                          "notice, warn, or error. (default: info)"}]] int              log_level = 2;
        [[= cli::option {"--no-log", "Disable logging to stdout (still writes to file)."}.flag()]] bool no_log = false;
        [[= cli::option {"--time", "Print total execution time at program finish."}.flag()]] bool       time   = false;
    };

    struct selection_options : common_options {
        // install modules
        static constexpr cli::section section4 = {"INSTALL MODULES"};
        [[= cli::option {"-I,--include <module...>",
                         "Install ONLY the specified modules <module>, semicolon-separated."}]] std::string include =
          "";
        [[= cli::option {"-x,--exclude <module...>",
                         "Install all modules EXCEPT specified <module>, semicolon-separated."}]] std::string exclude =
          "";
    };

    //===============================================================================================================
    // commands
    //===============================================================================================================
    struct[[= cli::command {"Install (or reinstall) available dotfiles."}]] install_cmd : selection_options {
        int operator()() const { return do_install(); }
    } install;

    struct[[= cli::command {"Update from Git before installing."}]] update_cmd : selection_options {
        int operator()() const {
            if (!do_update()) {
                // TODO: log
                prompt_continue("Error: update failed!");
            }
            return do_install();
        }
    } update;

    struct[[= cli::command {"Remove target modules <module>, semicolon-separated."}]] remove_cmd : common_options {
        [[= cli::argument {"Modules to remove, semicolon-separated."}]] std::string modules;
        int                                                                         operator()() const { return 0; }
    } remove;

    struct[[= cli::command {"Remove orphaned symlinks and generated files, then re-link/generate."}]] repair_cmd :
      common_options {
        int operator()() const { return 0; }
    } repair;

    struct[[= cli::command {
      "Remove all symlinks and generated files managed by this tool. (note: copied/merged files will remain)"}]]
      reset_cmd : common_options {
        int operator()() const { return 0; }
    } reset;

    struct[[= cli::command {"Deletes symlinks, logs, and backups, then deletes install directory."}]] uninstall_cmd :
      common_options {
        int operator()() const { return 0; }
    } uninstall;

    //===============================================================================================================
    // maintenance
    //===============================================================================================================
    static constexpr cli::section                                                    section2      = {"MAINTENANCE"};
    [[= cli::option {"--clean [N]", "Clean backups and reset both log files."}]] int clean         = 0;
    [[= cli::option {"--clean-logs", "Clean both log files."}.flag()]] bool          clean_logs    = false;
    [[= cli::option {"--clean-backups [N]",
                     "Clean backups. N: remove backup dirs older than N days. (default "
                     "when omitted: keep the latest "
                     "DOTFILES_MAX_BACKUPS dirs)"}]] int                             clean_backups = 0;

    //===============================================================================================================
    // information
    //===============================================================================================================
    static constexpr cli::section                                                 section3 = {"INFORMATION"};
    [[= cli::option {"--help", "Print this message and exit."}.flag()]] bool      help     = false;
    static constexpr std::string_view                                             version  = "v0.1.0";
    [[= cli::option {"-l,--list", "Display all available modules."}.flag()]] bool list     = false;

    //===========================================================================================================
    // environment variables
    //===========================================================================================================
    [[= cli::env {"",
                  "Path to store log files. (default: ~/.config/dotfiles/logs)"}]] cli::env_var<std::filesystem::path>
      dotfiles_log_dir = {"~/.config/dotfiles/logs"};
    [[= cli::env {"",
                  "Path to store temporary cache. (default: ~/.cache/dotfiles)"}]] cli::env_var<std::filesystem::path>
      dotfiles_cache_dir = {"~/.cache/dotfiles"};
    [[= cli::env {"", "Set to 0 or false to disable log file writing. (default: 1)"}]] cli::env_var<bool> dotfiles_log =
      {true};
    [[= cli::env {"", "Set to 1 or true to install with uncommitted local changes. (default: 0)"}]] cli::env_var<bool>
      dotfiles_local_mods = {false};
    [[= cli::env {"", "Set to 1 or true to restart shell at script finish. (default: 0)"}]] cli::env_var<bool>
      dotfiles_autorestart = {false};
    [[= cli::env {"", "Maximum number of backups to keep. Set to 0 to keep all. (default: 10)"}]] cli::env_var<int>
      dotfiles_max_backups = {10};

    // runs when invoked with no verb
    // int operator()() const { return 0; }
};
}  // namespace rs_dotfiles
