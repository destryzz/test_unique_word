MODULES = unique_words
EXTENSION = unique_words
DATA = unique_words--1.0.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)

include $(PGXS)