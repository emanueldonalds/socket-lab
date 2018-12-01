include Makefile.inc

GEN_EXE = server

LINUX_EXE = 

EXE = ${GEN_EXE} ${LINUX_EXE}

OBJS = main.cpp Transaction.cpp Transaction.h StockPosition.cpp StockPosition.h UserPosition.cpp UserPosition.h

all : ${EXE}

allgen : ${GEN_EXE}

server:
	${CC} -o server ${OBJS} ${CXXFLAGS}


CFLAGS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS}
LDLIBS = ${IMPL_LDLIBS} ${IMPL_THREAD_FLAGS}

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}