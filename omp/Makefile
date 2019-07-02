CPP = /usr/local/opt/llvm/bin/clang++
CPPFLAGS = -I/usr/local/opt/llvm/include -fopenmp
LDFLAGS = -L/usr/local/opt/llvm/lib -lsfml-graphics -lsfml-window -lsfml-system

life:life.cpp
	$(CPP) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)
clean:
	rm life