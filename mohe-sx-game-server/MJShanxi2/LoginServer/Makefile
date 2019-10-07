include Makefile.define
include Makefile.files
default:all

BASE_SRC = ../LBase/LActive.o \
  	    ../LBase/LCharge.o \
  	    ../LBase/LIni.o \
  	    ../LBase/LLog.o \
  	    ../LBase/LMsg.o \
  	    ../LBase/LNet.o \
  	    ../LBase/LRunnable.o \
  	    ../LBase/LSocket.o \
  	    ../LBase/LTabFile.o \
  	    ../LBase/LTime.o \
  	    ../LBase/LTool.o \
  	    ../LBase/LUser.o \
  	    ../LBase/LVideo.o \
  	    ../LBase/LVipLog.o \
		../LBase/Func.o	\
  	    ../LBase/BaseCommonFunc.o \
	    ../LBase/IPFilter.o \
	    ../LBase/LMemeoryRecycle.o \

BASE_LIBS = ./libs/LActive.o \
  	    ./libs/LCharge.o \
  	    ./libs/LIni.o \
  	    ./libs/LLog.o \
  	    ./libs/LMsg.o \
  	    ./libs/LNet.o \
  	    ./libs/LRunnable.o \
  	    ./libs/LSocket.o \
  	    ./libs/LTabFile.o \
  	    ./libs/LTime.o \
  	    ./libs/LTool.o \
  	    ./libs/LVideo.o \
  	    ./libs/LVipLog.o \
  	    ./libs/LUser.o \
  	    ./libs/Func.o \
  	    ./libs/BaseCommonFunc.o \
	    ./libs/IPFilter.o \
	    ./libs/LMemeoryRecycle.o


LIB_BASE_OBJS = $(BASE_LIBS) \
		 $(MODULE_LIBS) \


PATS = $(BASE_SRC) \
	$(MODULE_SRC) \




TARGET_BIN = XLLoginServer
COPY=@cp -f 
REMOVE=@\rm -rf
MAKE_PATH = mkdir

all: $(TARGET_BIN) Makefile 


#$(CC) -shared -o $(LIB_BASE) $(LIB_BASE_OBJS) $(LIBS) -lpthread  -Wl,zdefs,-Bsymbolic 
$(TARGET_BIN):$(PATS)
	$(REMOVE) ./libs
	$(MAKE_PATH) ./libs
	$(COPY) $(PATS) ./libs/
	$(CC) -o $(TARGET_BIN) $(LIB_BASE_OBJS) $(LIBS) -lpthread -fPIC -m64 -std=c++11 -lstdc++

clean:
	rm -rf *.o   $(LIB_BASE)  $(LIB_BASE_OBJS) $(PATH)   $(PATS)
	rm -rf $(TARGET_BIN)
	rm -rf ./libs

core:
	rm -rf core.* $(TARGET_BIN)
