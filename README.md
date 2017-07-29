# appindicator-collection

This project provides some libraries loaded with `LD_PRELOAD` which allow you to bring uniform look and behavior for application indicators.

## Building and Installing

Run `./autogen && ./configure --prefix=/usr/local && make && sudo make install` to install libraries.

You should also pass a list of desired applications to configure script (see below). By default all applications are excluded.

In order to enable modifications, you should run your application with `LD_PRELOAD` environment variable set to the path of shared library.

For example: `LD_PRELOAD=/usr/local/lib/libappindicator-hexchat.so hexchat`.

## Patch for Application Indicators

Optionally you can patch your libappindicator package with `libappindicator-activate.patch` patch from `tools` directory.
This patch adds `activate` action support so left click will open the application instead of the menu.

Note that this feature may be not supported by your desktop environment.

## Supported Applications

### Electron Applications

Configure flag: `--enable-electron`.  
Shared library: `libappindicator-electron.so`.

A hash sum is computed for each icon and appended to icon name. This allows you to replace icons using icon themes.

In order to get icon name use `sni-print` shell script from `tools` directory.

`ELECTRON_APPINDICATOR_TITLE` is optional environment variable which replaces the title of indicator.

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

- `telegram-tray` — regular icon
- `telegram-tray-new` — icon for unread messages
- `telegram-tray-highlight` — icon for direct private messages

Note that `XDG_CURRENT_DESKTOP` variable should be set to `Unity`!
