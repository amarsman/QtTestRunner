TEMPLATE = subdirs
SUBDIRS +=  src tests core

src.depends = core
tests.depends = core
tests.depends = src
