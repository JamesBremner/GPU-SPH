#source file search paths
VPATH = ./src 

	# compiler include search paths
INCS =  \
	-I../windex/include \


LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs -lComctl32 \


static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj

OPT = -O2
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_test = \
	$(ODIR)/test.o  \
	$(ODIR)/Particle.o  \


OBJ_noGL = \
	$(ODIR)/noGLmain.o  \
	$(ODIR)/cSPHsim.o  \
	$(ODIR)/Particle.o  \
	$(ODIR)/cGUI.o  \



### COMPILE

$(ODIR)/%.o: %.cpp  
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

test: $(OBJ_test)
	g++ $(OPT) \
	-o bin/test.exe $^  \
	$(static_link_flags) $(LIBS)

noGL: $(OBJ_noGL)
	g++ $(OPT) \
	-o bin/noGL.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

