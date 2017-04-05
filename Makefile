include config.inc

LIBS=
CFLAGS=-std=c++11 -Wall -Isrc

TARGET=$(BINDIR)/whitefield
# Should be equivalent to your list of C files, if you don't build selectively
SRCS=$(wildcard src/*.cc)
SRCS+=$(wildcard src/$(AIRLINK)/*.cc)
OBJS=$(addprefix $(BINDIR)/,$(SRCS:.cc=.o))
#$(info $(OBJS))
CFLAGS+=-Isrc/$(AIRLINK)

ifeq ($(AIRLINK),NS3)
CFLAGS+=-I$(NS3PATH)
LIBS+=-L$(NS3PATH) -lns3.26-core-$(REL) -lns3.26-lr-wpan-$(REL) -lns3.26-network-$(REL) -lns3.26-spectrum-$(REL) -lns3.26-mobility-$(REL) -lns3.26-propagation-$(REL) -lns3.26-stats-$(REL) -lns3.26-antenna-$(REL)
endif

ifeq ($(REL),debug)
CFLAGS+=-g
endif

all: $(BINDIR) $(TARGET)

$(TARGET): $(OBJS)
	g++ -o $(TARGET) $(OBJS) $(CFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cc %.h
	g++ -c -o $@ $< $(CFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cc
	g++ -c -o $@ $< $(CFLAGS) $(LIBS)

$(BINDIR):
	@mkdir -p $(BINDIR)/src/$(AIRLINK)

clean:
	@rm -rf $(BINDIR)
