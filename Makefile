INCLUDE = -I/usr/include/
LIBDIR  = -L/usr/X11R6/lib 

COMPILERFLAGS = -Wall
CC = g++
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab -lGLEW

fpv : FPV.o
	$(CC) FPV.o -o fpv $(LIBDIR) $(LIBRARIES)

FPV.o : FPV.cpp
	$(CC) $(CFLAGS) -c FPV.cpp -o FPV.o

clean :
	rm FPV.o fpv
