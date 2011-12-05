lib_LTLIBRARIES = libjess.la

libdir = @XMMS_VISUALIZATION_PLUGIN_DIR@
datadir = @XMMS_DATA_DIR@

libjess_la_LDFLAGS = -export-dynamic -avoid-version
libjess_la_LIBADD = @SDL_LIBS@
libjess_la_SOURCES = analyser.c analyser.h analyser_struct.h def.h distorsion.c distorsion.h draw.c draw.h draw_low_level.c draw_low_level.h jess.c jess.h misc.c misc.h palette.c palette.h projection.c projection.h renderer.c renderer.h struct.h
INCLUDES = @XMMS_CFLAGS@ @SDL_CFLAGS@
