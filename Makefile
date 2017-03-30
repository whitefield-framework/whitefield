LIBS=
CFLAGS=-std=c++11 -Wall -I.
BINDIR=bin
TARGET=$(BINDIR)/whitefield
# Should be equivalent to your list of C files, if you don't build selectively
SRCS=$(wildcard *.cc)
OBJS=$(addprefix $(BINDIR)/,$(SRCS:.cc=.o))
#$(addprefix $(BINDIR),$(OBJS))
#$(info $(OBJS))

all: $(BINDIR) $(TARGET)

$(TARGET): $(OBJS)
	g++ -o $(TARGET) $(OBJS) $(CFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cc
	g++ -c -o $@ $< $(CFLAGS) $(LIBS)

$(BINDIR):
	@mkdir -p $(BINDIR)

clean:
	@rm -rf $(BINDIR)
