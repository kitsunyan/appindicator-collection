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

### Electron Applications

Configure flag: `--enable-electron`.  
Shared library: `libappindicator-electron.so`.

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

### Telegram Desktop

Configure flag: `--enable-telegram`.  
Shared library: `libappindicator-telegram.so`.

The counter was moved to the tooltip.

The following icon names are used:

- `telegram-tray` — regular icon.
- `telegram-tray-new` — icon for unread messages.
- `telegram-tray-highlight` — icon for direct private messages.

Note that `XDG_CURRENT_DESKTOP` variable should be set to `Unity`!
