# This is a Makefile for generating OAuth RPC server and client

# Parameters
CLIENT = oauth_client
SERVER = oauth_server

SOURCES_CLNT.cpp = 
SOURCES_CLNT.h = 
SOURCES_SVC.cpp = 
SOURCES_SVC.h = 
SOURCES.x = oauth.x

TARGETS_SVC.cpp = oauth_svc.cpp oauth_server.cpp oauth_xdr.cpp 
TARGETS_CLNT.cpp = oauth_clnt.cpp oauth_client.cpp oauth_xdr.cpp 
TARGETS = oauth.h utils.h oauth_xdr.cpp oauth_clnt.cpp oauth_svc.cpp oauth_client.cpp oauth_server.cpp

OBJECTS_CLNT = $(SOURCES_CLNT.cpp:%.cpp=%.o) $(TARGETS_CLNT.cpp:%.cpp=%.o)
OBJECTS_SVC = $(SOURCES_SVC.cpp:%.cpp=%.o) $(TARGETS_SVC.cpp:%.cpp=%.o)

# Compiler and linker settings
CXX = g++
CXXFLAGS += -g -I/usr/include/tirpc
LDLIBS += -lnsl -ltirpc
RPCGENFLAGS = -C -a

# Default target
all: $(CLIENT) $(SERVER)

# Special target for rpc generation
generate: $(TARGETS)

# Rule to generate .cpp and .h files from .x file
$(TARGETS): $(SOURCES.x)
	rpcgen $(RPCGENFLAGS) $(SOURCES.x)
	mv -f $(SOURCES.x:%.x=%_svc.c) $(SOURCES.x:%.x=%_svc.cpp)
	mv -f $(SOURCES.x:%.x=%_clnt.c) $(SOURCES.x:%.x=%_clnt.cpp)
	mv -f $(SOURCES.x:%.x=%_xdr.c) $(SOURCES.x:%.x=%_xdr.cpp)
	mv -f $(SOURCES.x:%.x=%_client.c) $(SOURCES.x:%.x=%_client.cpp)
	mv -f $(SOURCES.x:%.x=%_server.c) $(SOURCES.x:%.x=%_server.cpp)
	sed -i '' '/register /d' $(TARGETS_CLNT_CPP) $(TARGETS_SVC_CPP) $(TARGETS_XDR_CPP)

# Compilation rules for client and server objects
$(OBJECTS_CLNT) : $(SOURCES_CLNT.cpp) $(SOURCES_CLNT.h) $(TARGETS_CLNT.cpp) 

$(OBJECTS_SVC) : $(SOURCES_SVC.cpp) $(SOURCES_SVC.h) $(TARGETS_SVC.cpp) 

# Linking rules for client and server executables
$(CLIENT) : $(OBJECTS_CLNT) 
	$(LINK.cpp) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS) 

$(SERVER) : $(OBJECTS_SVC) 
	$(LINK.cpp) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS)

# Clean-up rules
clean:
	$(RM) *.o $(CLIENT) $(SERVER)

.PHONY: all clean generate
