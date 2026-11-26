// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Pierce Katai <169690632+risingstarfish@users.noreply.github.com>


#include <print>
#include <string_view>

#include "dotfiles/dotfiles.hpp"

#include "thirdparty/simdjson/simdjson.h"

namespace {

constexpr const char registry[] = {
#embed "../modules/modules.json"
  ,
  0,
};

constexpr auto doc = simdjson::compile_time::parse_json<registry>();

static_assert(doc.platforms.size() == 6, "six platforms expected");
static_assert(doc.files.size() == 32, "thirty-two file entries expected");

}  // namespace

int main(int argc, const char **argv) {
    std::println("dotfiles — embedded registry: {} file entries, {} platforms", doc.files.size(), doc.platforms.size());

    std::println("\nplatforms (resolution chains):");
    for (const auto &p : doc.platforms) {
        std::println("  {:14} <- {}", std::string_view {p.name}, std::string_view {p.families});
    }

    std::println("\nmodules:");
    std::string_view current {};
    for (const auto &f : doc.files) {
        const std::string_view module {f.module};
        if (module != current) {
            current = module;
            std::size_t count {};
            for (const auto &g : doc.files) {
                count += std::string_view {g.module} == module;
            }
            std::println("  {:12} {} files", module, count);
        }
        std::println("    {:24} -> {} [{}]",
                     std::string_view {f.source},
                     std::string_view {f.destination},
                     std::string_view {f.type});
    }

    constexpr auto config = reflex::cli::configuration {
      .completion = {.enabled = false},
    };

    return reflex::cli::run<rs_dotfiles::dotfiles, config>(rs_dotfiles::dotfiles {}, argc, argv);
}
