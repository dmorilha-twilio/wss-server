CELLAR_OPENSSL_INCLUDE := $(shell find /usr/local/Cellar -path \*openssl/\*include | head -n 1;)
CELLAR_OPENSSL_LIB :=  $(CELLAR_OPENSSL_INCLUDE:/include=/lib)

CXXFLAGS += -std=c++11
CXXFLAGS += -I$(CELLAR_OPENSSL_INCLUDE)

LDFLAGS += -L$(CELLAR_OPENSSL_LIB)

LIBS += -lcrypto
LIBS += -lssl

wss-server: wss-server.cc
	${CXX} ${CXXFLAGS} ${LDFLAGS} ${LIBS} -o $@ $<;
	./$@;
