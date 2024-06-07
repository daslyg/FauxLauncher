DISCLAIMER: This executable is self-extracting; scripts are extracted and run upon execution. This can pose a security risk on some systems, most notably those with strict security policies. User discretion is advised.

Creates a Minecraft launcher shortcut, runs it, backs up your mods, resource packs, options (like keybinds and stuff like that), and shaderpacks upon closure of the launcher. When Minecraft java edition is run, it will wait for it to close before backing up all the worlds in your .minecraft directory, each timestamped. All backups created by the .exe are in the same directory, so changing your backup folder is as simple as moving the executable. A log is created for debugging purposes.

Warning, .zips may be overwritten if they share the same name in your backup folder when the executable is run, and the executable is HARDCODED to work (as expected) when the launcher is set to stay open on game launch.

Requirements:
Windows 11, [Minecraft](https://www.minecraft.net/), [7-Zip](https://www.7-zip.org/)

Downloads are available in the releases page.

See also: [FalsLauncher (Vintage Story)](https://github.com/daslyg/FalsLauncher)
