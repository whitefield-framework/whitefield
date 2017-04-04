LIBS=
CFLAGS=-g -std=c++11 -Wall -I.
BINDIR=bin
AIRLINK=NS3

TARGET=$(BINDIR)/whitefield
# Should be equivalent to your list of C files, if you don't build selectively
SRCS=$(wildcard *.cc)
SRCS+=$(wildcard $(AIRLINK)/*.cc)
OBJS=$(addprefix $(BINDIR)/,$(SRCS:.cc=.o))
#$(info $(OBJS))
CFLAGS+=-I$(AIRLINK)

all: $(BINDIR) $(TARGET)

$(TARGET): $(OBJS)
	g++ -o $(TARGET) $(OBJS) $(CFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cc
	g++ -c -o $@ $< $(CFLAGS) $(LIBS)

$(BINDIR):
	@mkdir -p $(BINDIR)/$(AIRLINK)

clean:
	@rm -rf $(BINDIR)
