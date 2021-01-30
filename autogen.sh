#!/bin/sh
autoreconf -v --force --install || exit $?
[ "$NOCONFIGURE" ] || exec ./configure "$@"
