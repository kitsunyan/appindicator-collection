AM_CPPFLAGS = \
	-I${top_srcdir} \
	$(PLATFORM_CPPFLAGS)

AM_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	-shared \
	$(PLATFORM_LDFLAGS)

installdir = \
	$(libdir)/indicators

install_LTLIBRARIES =

if ENABLE_ACTIVATE_GTK2

install_LTLIBRARIES += \
	libactivate-gtk2.la

libactivate_gtk2_la_SOURCES = \
	activate.c

libactivate_gtk2_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libactivate_gtk2_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libhashmapping.la \
	$(top_builddir)/common/libappindicator-gtk2.la

endif

if ENABLE_ACTIVATE_GTK3

install_LTLIBRARIES += \
	libactivate-gtk3.la

libactivate_gtk3_la_SOURCES = \
	activate.c

libactivate_gtk3_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

libactivate_gtk3_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libhashmapping.la \
	$(top_builddir)/common/libappindicator-gtk3.la

endif

if ENABLE_ELECTRON_GTK2

install_LTLIBRARIES += \
	libelectron-gtk2.la

libelectron_gtk2_la_SOURCES = \
	electron.c

libelectron_gtk2_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libelectron_gtk2_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libhashmapping.la \
	$(top_builddir)/common/libappindicator-gtk2.la

endif

if ENABLE_ELECTRON_GTK3

install_LTLIBRARIES += \
	libelectron-gtk3.la

libelectron_gtk3_la_SOURCES = \
	electron.c

libelectron_gtk3_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

libelectron_gtk3_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libhashmapping.la \
	$(top_builddir)/common/libappindicator-gtk3.la

endif

if ENABLE_GAJIM

install_LTLIBRARIES += \
	libgajim.la

libgajim_la_SOURCES = \
	gajim.c

libgajim_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libgajim_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libappindicator-gtk2.la \
	$(top_builddir)/common/libiconstub-gtk2.la

endif

if ENABLE_HEXCHAT

install_LTLIBRARIES += \
	libhexchat.la

libhexchat_la_SOURCES = \
	hexchat.c

libhexchat_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libhexchat_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libappindicator-gtk2.la \
	$(top_builddir)/common/libiconstub-gtk2.la

endif

if ENABLE_SYSTRAY_GTK2

install_LTLIBRARIES += \
	libsystray-gtk2.la

libsystray_gtk2_la_SOURCES = \
	systray.c

libsystray_gtk2_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libsystray_gtk2_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libhashmapping.la \
	$(top_builddir)/common/libappindicator-gtk2.la \
	$(top_builddir)/common/libiconstub-gtk2.la

endif

if ENABLE_SYSTRAY_GTK3

install_LTLIBRARIES += \
	libsystray-gtk3.la

libsystray_gtk3_la_SOURCES = \
	systray.c

libsystray_gtk3_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

libsystray_gtk3_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libhashmapping.la \
	$(top_builddir)/common/libappindicator-gtk3.la \
	$(top_builddir)/common/libiconstub-gtk3.la

endif

if ENABLE_TELEGRAM

install_LTLIBRARIES += \
	libtelegram.la

libtelegram_la_SOURCES = \
	telegram.c

libtelegram_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

libtelegram_la_LIBADD = \
	$(top_builddir)/common/libcommon.la \
	$(top_builddir)/common/libappindicator-gtk3.la

endif
