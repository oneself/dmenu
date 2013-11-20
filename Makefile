# dmenu - dynamic menu
# See LICENSE file for copyright and license details.

include config.mk

SRC = dmenu.c draw.c stest.c dmenu_mru.c mru.c
OBJ = ${SRC:.c=.o}

all: options dmenu stest dmenu_mru

options:
	@echo dmenu build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

debug: CFLAGS += -g
debug: all

.c.o:
	@echo CC -c $<
	@${CC} -c $< ${CFLAGS}

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

${OBJ}: config.h config.mk draw.h

dmenu: dmenu.o draw.o
	@echo CC -o $@
	@${CC} -o $@ dmenu.o draw.o ${LDFLAGS}

stest: stest.o
	@echo CC -o $@
	@${CC} -o $@ stest.o ${LDFLAGS}

dmenu_mru: dmenu_mru.o mru.o
	@echo CC -o $@
	@${CC} -o $@ dmenu_mru.o mru.o ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f dmenu stest dmenu_mru ${OBJ} dmenu-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p dmenu-${VERSION}
	@cp LICENSE Makefile README config.mk dmenu.1 draw.h mru.h dmenu_path dmenu_run dmenu_do stest.1 ${SRC} dmenu-${VERSION}
	@tar -cf dmenu-${VERSION}.tar dmenu-${VERSION}
	@gzip dmenu-${VERSION}.tar
	@rm -rf dmenu-${VERSION}

install: all
	@echo installing executables to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f dmenu dmenu_path dmenu_mru dmenu_run dmenu_do stest ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/dmenu
	@chmod 755 ${DESTDIR}${PREFIX}/bin/dmenu_path
	@chmod 755 ${DESTDIR}${PREFIX}/bin/dmenu_mru
	@chmod 755 ${DESTDIR}${PREFIX}/bin/dmenu_run
	@chmod 755 ${DESTDIR}${PREFIX}/bin/dmenu_do
	@chmod 755 ${DESTDIR}${PREFIX}/bin/stest
	@echo installing manual pages to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < dmenu.1 > ${DESTDIR}${MANPREFIX}/man1/dmenu.1
	@sed "s/VERSION/${VERSION}/g" < stest.1 > ${DESTDIR}${MANPREFIX}/man1/stest.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/dmenu.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/stest.1

uninstall:
	@echo removing executables from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/dmenu
	@rm -f ${DESTDIR}${PREFIX}/bin/dmenu_path
	@rm -f ${DESTDIR}${PREFIX}/bin/dmenu_mru
	@rm -f ${DESTDIR}${PREFIX}/bin/dmenu_run
	@rm -f ${DESTDIR}${PREFIX}/bin/dmenu_do
	@rm -f ${DESTDIR}${PREFIX}/bin/stest
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/dmenu.1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/stest.1

.PHONY: all options clean dist install uninstall
