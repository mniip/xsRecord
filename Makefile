CXX= g++
MOC= moc

OBJECTS= Main.o PPM.o Queue.o X.o AppWindow.o AppWindow.moc.o
OUT= xsrecord

QTCFLAGS= -I/usr/include/qt4/
QTLDFLAGS= -L/usr/lib/qt4/ -lQtCore -lQtGui

CFLAGS= -O2 $(QTCFLAGS) $(MYCFLAGS)
LDFLAGS= -lX11 -pthread $(QTLDFLAGS) $(MYLDFLAGS)

all: $(OUT)

%.moc.cpp: %.h
	$(MOC) -o $@ $<

%.o: %.cpp
	$(CXX) -c -o $@ $(CFLAGS) $<

Main.cpp: AppWindow.moc.cpp

$(OUT): $(OBJECTS)
	$(CXX) -o $@ $(LDFLAGS) $+

clean:
	rm -f $(OBJECTS) $(OUT) AppWindow.moc.cpp
