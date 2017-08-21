# appindicator-collection

This project provides some libraries loaded with `LD_PRELOAD` which allow you to bring uniform look and behavior for application indicators.

## Building and Installing

Run `./autogen && ./configure --prefix=/usr/local && make && sudo make install` to install libraries.

You should also pass a list of desired applications to configure script (see below). By default all applications are excluded.

In order to enable modifications, you should run your application with `LD_PRELOAD` environment variable set to the path of shared library.

For example: `LD_PRELOAD=/usr/local/lib/libappindicator-hexchat.so hexchat`.

## Left-Click Activation

Optionally you can override some indicator methods to allow left-click activation using `--with-activate` configure option.

Note that this feature may be not supported by your desktop environment.

## Supported Applications

### Activated Indicators

Configure flag: `--enable-activate`.  
Shared library (GTK+ 2): `libappindicator-activate-gtk2.so`.  
Shared library (GTK+ 3): `libappindicator-activate-gtk3.so`.

Generic patch for GTK+ applications with application indicator which adds support for `activate` action.
This will work if your application provide a secondary activation method.

You can also specify the index of menu item which will be used as `activate` target using `ACTIVATE_APPINDICATOR_INDEX`
environment variable. Negative index may be used to select menu item from the end of menu.

This library may be useful if you want to simply add left click support for application indicators.

### Electron Applications

Configure flag: `--enable-electron`.  
Shared library (GTK+ 2): `libappindicator-electron-gtk2.so`.  
Shared library (GTK+ 3): `libappindicator-electron-gtk3.so`.

A hash sum is computed for each icon and appended to icon name. This allows you to replace icons using icon themes.

In order to get icon name use `sni-print` shell script from `tools` directory.

Environment variables:

- `ELECTRON_APPINDICATOR_TITLE` — allows to replace indicator title.
- `ELECTRON_MENU_HEAD_ACTIVATE` — first menu item will be used for `activate` action.

### Gajim

Configure flag: `--enable-gajim`.  
Shared library: `libappindicator-gajim.so`.

This patch replaces system tray icon with application indicator.

The following icon names are used:

- `gajim-tray-[status]` — regular icon for specific `[status]`.
- `gajim-tray-[status]-highlight` — highlight icon for specific `[status]`.

`[status]` can take the following values:
`away`, `chat`, `connecting`, `dnd`, `error`, `invisible`, `offline`, `online`, `xa`.  
For example: `gajim-tray-online-highlight`.

### HexChat

Configure flag: `--enable-hexchat`.  
Shared library: `libappindicator-hexchat.so`.

This patch replaces system tray icon with application indicator.

The following icon names are used:

- `hexchat-tray` — regular icon
- `hexchat-tray-new` — blinking icon for new messages

### System Tray

Configure flag: `--enable-systray`.  
Shared library (GTK+ 2): `libappindicator-systray-gtk2.so`.  
Shared library (GTK+ 3): `libappindicator-systray-gtk3.so`.

Generic patch for GTK+ applications which replaces systray with application indicator.

A different icon name is used for different types of icons. Use `sni-print` shell script from `tools` directory
to determine icon name.

This library may be useful for applications with static systray icons.

### Telegram Desktop

Configure flag: `--enable-telegram`.  
Shared library: `libappindicator-telegram.so`.

The counter was moved to the tooltip.

The following icon names are used:

- `telegram-tray` — regular icon.
- `telegram-tray-new` — icon for unread messages.
- `telegram-tray-highlight` — icon for direct private messages.

Note that `XDG_CURRENT_DESKTOP` variable should be set to `Unity`!

## Additional Features

### Hash Code Mapping

You may notice that a hash code is appended to icon name for `systray` and `electron` applications.
Hash code can be mapped to human-readable string using `APPINDICATOR_HASH_MAPPING` environment variable.

This variable should contain an array of key-value pairs where key is hash code and value is human-readable string.
Pairs are divided by `/` character, keys and values are divided by `=` character.
An extra `*` key is used to transform hash codes which aren't listed in your array.

Example for Riot Desktop application: `6695197e=tray/*=tray-highlight`.
`riot-tray` will be used for regular icon, `riot-tray-highlight` will be used for other icons
(for new messages, errors, etc).
