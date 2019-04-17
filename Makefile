# contrib/tanimoto/Makefile

MODULE_big = tanimoto
OBJS = tanimoto.o 

EXTENSION = tanimoto
DATA = tanimoto--1.0.sql
PGFILEDESC = "tanimoto - calculate Tanimoto coefficient for bit varying"

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/tanimoto
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
