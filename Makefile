# Makefile for hev-scgi-server
 
PP=cpp
CC=cc
GIR_SCANNER=g-ir-scanner
GIR_COMPILER=g-ir-compiler
VAPIGEN=vapigen
PKG_DEPS=glib-2.0 gmodule-2.0 gio-2.0 gio-unix-2.0
CCFLAGS=-O3 -fPIC `pkg-config --cflags $(PKG_DEPS)`
LDFLAGS=-shared `pkg-config --libs $(PKG_DEPS)`
 
SRCDIR=src
BINDIR=bin
BUILDDIR=build
 
TARGET=$(BINDIR)/libhev-scgi-server.so
GIR_FILE=gir/HevSCGI-1.0.gir
GIR_TYPELIB=gir/HevSCGI-1.0.typelib
VAPI_FILE=vapi/hev-scgi-1.0.vapi
GIR_SOURCES= src/hev-scgi-server.c src/hev-scgi-server.h \
		src/hev-scgi-handler.c src/hev-scgi-handler.h \
		src/hev-scgi-request.c src/hev-scgi-request.h \
		src/hev-scgi-response.c src/hev-scgi-response.h \
		src/hev-scgi-task.c src/hev-scgi-task.h
CCOBJSFILE=$(BUILDDIR)/ccobjs
-include $(CCOBJSFILE)
LDOBJS=$(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o,$(CCOBJS))
 
DEPEND=$(LDOBJS:.o=.dep)
 
all : $(CCOBJSFILE) $(TARGET) $(GIR_FILE) $(GIR_TYPELIB) $(VAPI_FILE)
	@$(RM) $(CCOBJSFILE)
 
clean : 
	@echo -n "Clean ... " && $(RM) -f $(BINDIR)/* $(BUILDDIR)/* \
		$(GIR_FILE) $(GIR_TYPELIB) $(VAPI_FILE) && echo "OK"
 
$(CCOBJSFILE) : 
	@echo CCOBJS=`ls $(SRCDIR)/*.c` > $(CCOBJSFILE)
 
$(TARGET) : $(LDOBJS)
	@echo -n "Linking $^ to $@ ... " && $(CC) -o $@ $^ $(LDFLAGS) && echo "OK"

$(GIR_FILE) : $(TARGET) $(GIR_SOURCES)
	@echo -n "Generating $@ ..." && LD_LIBRARY_PATH=./bin \
		$(GIR_SCANNER) --quiet -l hev-scgi-server -L bin \
		-n HevSCGI --nsversion=1.0 --c-include hev-scgi-1.0.h \
		-i GLib-2.0 -i GObject-2.0 -i Gio-2.0 \
		--identifier-prefix=HevSCGI --symbol-prefix=hev_scgi \
		-o $@ $(GIR_SOURCES) &>/dev/null && echo "OK"

$(GIR_TYPELIB) : $(GIR_FILE)
	@echo -n "Generating $@ ..." && $(GIR_COMPILER) -o $@ $^ && echo "OK"

$(VAPI_FILE) : $(GIR_FILE)
	@echo -n "Generating $@ ..." && \
		$(VAPIGEN) -q --pkg=glib-2.0 --pkg=gobject-2.0 --pkg=gio-2.0 \
		--library=hev-scgi-1.0 -d vapi $^ && echo "OK"
 
$(BUILDDIR)/%.dep : $(SRCDIR)/%.c
	@$(PP) $(CCFLAGS) -MM -MT $(@:.dep=.o) -o $@ $<
 
$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@echo -n "Building $< ... " && $(CC) $(CCFLAGS) -c -o $@ $< && echo "OK"
 
-include $(DEPEND)

