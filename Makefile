# Makefile for hev-scgi-server-library

PROJECT=hev-scgi-server-library
 
CROSS_PREFIX :=
PP=$(CROSS_PREFIX)cpp
CC=$(CROSS_PREFIX)gcc
AR=$(CROSS_PREFIX)ar
LD=$(CROSS_PREFIX)ld
STRIP=$(CROSS_PREFIX)strip
GIR_SCANNER=g-ir-scanner
GIR_COMPILER=g-ir-compiler
VAPIGEN=vapigen
PKG_DEPS=glib-2.0 gmodule-2.0 gio-2.0 gio-unix-2.0
CCFLAGS=-O3 -Werror -Wall `pkg-config --cflags $(PKG_DEPS)`
LDFLAGS=
 
SRCDIR=src
BINDIR=bin
BUILDDIR=build
 
STATIC_TARGET=$(BINDIR)/libhev-scgi-server.a
SHARED_TARGET=$(BINDIR)/libhev-scgi-server.so

$(STATIC_TARGET): CCFLAGS+=-DSTATIC_MODULE
$(SHARED_TARGET): CCFLAGS+=-fPIC
$(SHARED_TARGET): LDFLAGS+=-shared `pkg-config --libs $(PKG_DEPS)`

GIR_FILE=gir/HevSCGI-1.0.gir
GIR_TYPELIB=gir/HevSCGI-1.0.typelib
VAPI_FILE=vapi/hev-scgi-1.0.vapi
GIR_SOURCES= src/hev-scgi-server.c src/hev-scgi-server.h \
		src/hev-scgi-handler.c src/hev-scgi-handler.h \
		src/hev-scgi-request.c src/hev-scgi-request.h \
		src/hev-scgi-response.c src/hev-scgi-response.h \
		src/hev-scgi-task.c src/hev-scgi-task.h
CCOBJS = $(wildcard $(SRCDIR)/*.c)
LDOBJS = $(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o,$(CCOBJS))
DEPEND = $(LDOBJS:.o=.dep)

BUILDMSG="\e[1;31mBUILD\e[0m $<"
LINKMSG="\e[1;34mLINK\e[0m  \e[1;32m$@\e[0m"
STRIPMSG="\e[1;34mSTRIP\e[0m \e[1;32m$@\e[0m"
GENMSG="\e[1;31mGEN\e[0m   $@"
CLEANMSG="\e[1;34mCLEAN\e[0m $(PROJECT)"

V :=
ECHO_PREFIX := @
ifeq ($(V),1)
	undefine ECHO_PREFIX
endif

shared : $(SHARED_TARGET) $(GIR_FILE) $(GIR_TYPELIB) $(VAPI_FILE)
 
static : $(STATIC_TARGET)
 
clean : 
	$(ECHO_PREFIX) $(RM) $(BINDIR)/* $(BUILDDIR)/* \
		$(GIR_FILE) $(GIR_TYPELIB) $(VAPI_FILE)
	@echo -e $(CLEANMSG)
 
$(STATIC_TARGET) : $(LDOBJS)
	$(ECHO_PREFIX) $(AR) csq $@ $^
	@echo -e $(LINKMSG)

$(SHARED_TARGET) : $(LDOBJS)
	$(ECHO_PREFIX) $(CC) -o $@ $^ $(LDFLAGS)
	@echo -e $(LINKMSG)
	$(ECHO_PREFIX) $(STRIP) $@
	@echo -e $(STRIPMSG)

$(GIR_FILE) : $(TARGET) $(GIR_SOURCES)
	$(ECHO_PREFIX) LD_LIBRARY_PATH=./bin \
		$(GIR_SCANNER) --quiet -l hev-scgi-server -L bin \
		-n HevSCGI --nsversion=1.0 --c-include hev-scgi-1.0.h \
		-i GLib-2.0 -i GObject-2.0 -i Gio-2.0 \
		--identifier-prefix=HevSCGI --symbol-prefix=hev_scgi \
		-o $@ $(GIR_SOURCES) &>/dev/null
	@echo -e $(GENMSG)

$(GIR_TYPELIB) : $(GIR_FILE)
	$(ECHO_PREFIX) $(GIR_COMPILER) -o $@ $^
	@echo -e $(GENMSG)

$(VAPI_FILE) : $(GIR_FILE)
	$(ECHO_PREFIX) $(VAPIGEN) -q --pkg=glib-2.0 --pkg=gobject-2.0 --pkg=gio-2.0 \
		--library=hev-scgi-1.0 -d vapi $^
	@echo -e $(GENMSG)
 
$(BUILDDIR)/%.dep : $(SRCDIR)/%.c
	$(ECHO_PREFIX) $(PP) $(CCFLAGS) -MM -MT $(@:.dep=.o) -o $@ $<
 
$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	$(ECHO_PREFIX) $(CC) $(CCFLAGS) -c -o $@ $<
	@echo -e $(BUILDMSG)
 
-include $(DEPEND)

