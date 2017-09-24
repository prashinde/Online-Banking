EXEC := server client
BIN_DIR := bin
MKDIR_P := mkdir -p 

s_INCLUDE_DIRS := server utils
s_CXX_SRCS := $(foreach idir, $(s_INCLUDE_DIRS), $(wildcard $(idir)/*.cpp))
s_CXX_OBJS := ${s_CXX_SRCS:.cpp=.o}
s_OBJS := $(s_CXX_OBJS)

c_INCLUDE_DIRS := client utils
c_CXX_SRCS := $(foreach idir, $(c_INCLUDE_DIRS), $(wildcard $(idir)/*.cpp))
c_CXX_OBJS := ${c_CXX_SRCS:.cpp=.o}
c_OBJS := $(c_CXX_OBJS)

cs_LIBRARIES := pthread

CPPFLAGS += $(foreach includedir,$(s_INCLUDE_DIRS),-I$(includedir))
CPPFLAGS += $(foreach includedir,$(c_INCLUDE_DIRS),-I$(includedir))
#LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(cs_LIBRARIES),-$(library))

.PHONY: all clean distclean

all: BIN_DIR $(EXEC)

BIN_DIR: ${BIN_DIR}
${BIN_DIR}:
	${MKDIR_P} ${BIN_DIR}

server: $(s_OBJS)
	$(LINK.cc) $(s_OBJS) -o $(BIN_DIR)/$@

client: $(c_OBJS)
	$(LINK.cc) $(c_OBJS) -o $(BIN_DIR)/$@

clean:
	${foreach t, ${EXEC},$(RM) $(BIN_DIR)/$(t)}
	$(RM) $(s_OBJS)
	$(RM) $(c_OBJS)
	$(RM) -r ${BIN_DIR}

distclean: clean
