AM_CPPFLAGS = \
	-I${top_srcdir} \
	$(PLATFORM_CPPFLAGS)

noinst_LTLIBRARIES = \
	libcommon.la

libcommon_la_SOURCES = \
	common.c \
	common.h

libcommon_la_LIBADD = \
	-ldl

libcommon_la_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)
