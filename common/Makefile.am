AM_CPPFLAGS = \
	-I${top_srcdir} \
	$(PLATFORM_CPPFLAGS)

noinst_LTLIBRARIES = \
	libcommon.la \
	libhashmapping.la

libcommon_la_SOURCES = \
	common.c \
	common.h

libcommon_la_LIBADD = \
	-ldl

libhashmapping_la_SOURCES = \
	hash-mapping.c \
	hash-mapping.h

libhashmapping_la_CFLAGS = \
	$(GLIB2_CFLAGS)

libhashmapping_la_LIBADD = \
	$(GLIB2_LIBS)

if ENABLE_GTK2

noinst_LTLIBRARIES += \
	libappindicator-gtk2.la \
	libiconstub-gtk2.la

libappindicator_gtk2_la_SOURCES = \
	app-indicator.c \
	app-indicator.h

libappindicator_gtk2_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libappindicator_gtk2_la_LIBADD = \
	$(APP_INDICATOR_GTK2_LIBS)

libiconstub_gtk2_la_SOURCES = \
	icon-stub.c \
	icon-stub.h

libiconstub_gtk2_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

endif

if ENABLE_GTK3

noinst_LTLIBRARIES += \
	libappindicator-gtk3.la \
	libiconstub-gtk3.la

libappindicator_gtk3_la_SOURCES = \
	app-indicator.c \
	app-indicator.h

libappindicator_gtk3_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

libappindicator_gtk3_la_LIBADD = \
	$(APP_INDICATOR_GTK3_LIBS)

libiconstub_gtk3_la_SOURCES = \
	icon-stub.c \
	icon-stub.h

libiconstub_gtk3_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

endif
