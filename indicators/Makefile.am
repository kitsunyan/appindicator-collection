AM_CPPFLAGS = \
	-I${top_srcdir} \
	$(PLATFORM_CPPFLAGS)

installdir = \
	$(libdir)

install_LTLIBRARIES =

if ENABLE_ELECTRON

install_LTLIBRARIES += \
	libappindicator-electron-gtk2.la \
	libappindicator-electron-gtk3.la

libappindicator_electron_gtk2_la_SOURCES = \
	electron.c

libappindicator_electron_gtk2_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libappindicator_electron_gtk2_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(APP_INDICATOR_GTK2_LIBS)

libappindicator_electron_gtk2_la_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

libappindicator_electron_gtk3_la_SOURCES = \
	electron.c

libappindicator_electron_gtk3_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

libappindicator_electron_gtk3_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(APP_INDICATOR_GTK3_LIBS)

libappindicator_electron_gtk3_la_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

endif

if ENABLE_HEXCHAT

install_LTLIBRARIES += \
	libappindicator-hexchat.la

libappindicator_hexchat_la_SOURCES = \
	hexchat.c

libappindicator_hexchat_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libappindicator_hexchat_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(APP_INDICATOR_GTK2_LIBS)

libappindicator_hexchat_la_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

endif

if ENABLE_TELEGRAM

install_LTLIBRARIES += \
	libappindicator-telegram.la

libappindicator_telegram_la_SOURCES = \
	telegram.c

libappindicator_telegram_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

libappindicator_telegram_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(APP_INDICATOR_GTK3_LIBS)

libappindicator_telegram_la_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

endif
