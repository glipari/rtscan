VERSION=$(shell grep version taskgen.py | sed -e 's/.*version \([0-9]\+\.[0-9]\+\).*/\1/')

DIST_NAME=taskgen-$(VERSION)
DIST_FILES=README COPYRIGHT taskgen.py Makefile

# Default installation
prefix=/usr/local
bindir=$(prefix)/bin

all:
	@echo ""
	@echo "Please, use one of the following targets:"
	@echo ""
	@echo "  make install               Install taskgen.py in $(bindir)"
	@echo "                               (needs root privileges)"
	@echo "  make install bindir=<path> Install taskgen.py in <path>"
	@echo "  make dist                  Create a distribution package"
	@echo ""

install:
	cp taskgen.py $(bindir)

dist:
	mkdir /tmp/$(DIST_NAME) && cp -a $(DIST_FILES) /tmp/$(DIST_NAME) && cd /tmp && tar -czf $(DIST_NAME).tar.gz $(DIST_NAME) && echo "Distribution archive built into /tmp/$(DIST_NAME).tar.gz"
