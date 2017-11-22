CXX=g++
DEBUG=1
CFLAGS=-std=c++11 -O3 -framework OpenGL -I/usr/local/include/ -lglfw3 `pkg-config --cflags opencv` -DDEBUG

LDFLAGS=-framework OpenGL -lglfw3 `pkg-config --libs opencv` -lGLEW


OBJS=main.o VBO.o TRIModel.o Transform.o Light.o Shader.o controls.o

main: $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)

clean:
	rm -f  $(OBJS) main
