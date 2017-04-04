LIBS=
CFLAGS=-g -std=c++11 -Wall -Isrc
BINDIR=bin
AIRLINK=NS3

TARGET=$(BINDIR)/whitefield
# Should be equivalent to your list of C files, if you don't build selectively
SRCS=$(wildcard src/*.cc)
SRCS+=$(wildcard src/$(AIRLINK)/*.cc)
OBJS=$(addprefix $(BINDIR)/,$(SRCS:.cc=.o))
#$(info $(OBJS))
CFLAGS+=-Isrc/$(AIRLINK)

all: $(BINDIR) $(TARGET)

$(TARGET): $(OBJS)
	g++ -o $(TARGET) $(OBJS) $(CFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cc
	g++ -c -o $@ $< $(CFLAGS) $(LIBS)

$(BINDIR):
	@mkdir -p $(BINDIR)/src/$(AIRLINK)

clean:
	@rm -rf $(BINDIR)
