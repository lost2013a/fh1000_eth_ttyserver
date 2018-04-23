# 府河FH1000串口转可为私有协议
#
VPATH = -I ./ \
          
Objects =  main.o manager.o ttyS1000.o kwserver.o ComUnit.o Report.o comminterface.o CComPort.o deviceAttach.o tinyxml2.o

LIBS = 

LINK =  -fPIC -lpthread -lz -ldl -lm -lrt -lnsl
																															
CFLAGS = -O2 -w -DOS_LINUX -DQ_OS_UNIX  -fvisibility=hidden

LFLAGS  = 

#CC =  arm-linux-g++
#用-uclibc只是强调运行环境在uclibc下，不兼容与常用的arm-linux交叉编译链
CC =  arm-linux-uclibc-g++

VERSION=1.1.2

TARGET        = ../bin/ttyServer-${VERSION}
TARGET0       = ttyServer

$(TARGET) : $(Objects) mklibdir
	$(CC) $(LFLAGS) $(LINK) -o $(TARGET) $(Objects)  $(LIBS) $(CFLAGS) $(VPATH) 
#	-ln -s $(TARGET) $(TARGET0)
#	mv -f $(TARGET) $(TARGET0) ../bin/
	

main.o : main.cpp
	 $(CC) -o main.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c main.cpp	   
	     
manager.o : manager.cpp
	 $(CC) -o manager.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c manager.cpp	   
	     
ttyS1000.o : ttyS1000.cpp
	 $(CC) -o ttyS1000.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c ttyS1000.cpp    
	     	  
kwserver.o : kwserver.cpp
	 $(CC) -o kwserver.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c kwserver.cpp    

ComUnit.o : ComUnit.cpp
	 $(CC) -o ComUnit.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c ComUnit.cpp    
	     	  	     	  
Report.o : ../common/Report.cpp
	 $(CC) -o Report.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c ../common/Report.cpp    

comminterface.o : ../common/comminterface.cpp
	 $(CC) -o comminterface.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c ../common/comminterface.cpp   

CComPort.o : CComPort.cpp
	 $(CC) -o CComPort.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c CComPort.cpp	   	     	     	     	     	     	      	  

deviceAttach.o : deviceAttach.cpp
	 $(CC) -o deviceAttach.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c deviceAttach.cpp	 
	     
tinyxml2.o : tinyxml2.cpp
	 $(CC) -o tinyxml2.o $(LIBS) $(CFLAGS) $(VPATH) \
	     -c tinyxml2.cpp	  	       	     	     	     	     	     	      	  
	     	     	     	     	  	     	     	     	     	     	      	     	     	          	     	     	     	     	     	     	     	     	     	     
mklibdir:
	if [ -d ../bin/ ]; then echo "../bin/ exists";   else mkdir -p ../bin/; fi

.PHONY : install
install:
	@echo nothing done

.PHONY : print
print:
	@echo nothing done

.PHONY : tar
tar:
	@echo nothing done

.PHONY : clean

clean :
	-rm *.o $(TARGET) $(TARGET0) $(Objects)
