AUTOMAKE_OPTION=foreign
bin_PROGRAMS=dandelion
dandelion_SOURCES=main.cpp auth.cpp parser.cpp request.cpp robot.cpp util_string.cpp weibo_api.cpp weibo_param.cpp log.cpp
dandelion_LDADD=$(top_srcdir)/net/libnet.a	\
		$(top_srcdir)/url/liburl.a	\
		-liconv -lpthread -lcurl -lssl -lcrypto

#dandelion_LDFLAGS=-static
SUBDIRS=net url
CURRENTPATH=$(pwd)
INCLUDES=-I./ -I./include
export INCLUDES
