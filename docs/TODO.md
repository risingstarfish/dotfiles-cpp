# TODO

## Planned modules (salvaged from the retired configs/default.modules.json)

Sources referenced below never existed in this repo — port the files first,
then add the module directory + entries in `modules/modules.json`.

- **bash** — `bashrc` (was `copy` to `~/.bashrc`)
- **pwsh** — `windows.ps1` → `~/Documents/Powershell/Microsoft.PowerShell_profile.ps1`;
  `functions/` dir → `~/Documents/Powershell/Scripts/Functions`;
  `Set-MSVC-Environment.ps1` → `~/Documents/Powershell/Scripts/`
- **fastfetch** — `config.jsonc` → `~/.config/fastfetch/config.jsonc`
- **claude** — `settings.json` → `~/.claude/settings.json`; `plugin.json` → `~/.claude/plugin.json`
- **oh-my-posh** — `themes/` dir → `~/.oh-my-posh/themes` (was `windows` only)
- **zsh** — WSL helper `Set-MSVC-Environment` (was declared as `functions/Set-MSVC-Environment`
  but never existed; `functions` is a single file)

## Loose ends

- `zshrc` logs `ERROR` when `~/.paths.local` is missing, but nothing installs it —
  ship a `paths.local` template/overlay in the zsh module, or demote the message.
- Per-module `modules/*/module.json` manifests pending deletion (their `$schema`
  points at the deleted `module.schema.json`; the registry `modules/modules.json`
  is the single source). None remain on disk — verify and drop this bullet.
