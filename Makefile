CXXFLAGS = -Wall -std=c++11 -fopenmp
MAIN_DEPS = main.cpp drawer.o geometry.o raytracer.o vec3.o loader.o kdtree.o
DRAW_DEPS = drawer.cpp raytracer.cpp vec3.cpp geometry.cpp kdtree.cpp
GEO_DEPS = geometry.cpp vec3.cpp 
LOAD_DEPS = loader.cpp geometry.cpp vec3.cpp
VEC_DEPS = vec3.cpp  
KD_DEPS = kdtree.cpp vec3.cpp geometry.cpp
RAY_DEPS = raytracer.cpp vec3.cpp geometry.cpp kdtree.cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@ 

kdtree.o: $(KD_DEPS)

loader.o: $(LOAD_DEPS)

vec3.o: $(VEC_DEPS)

geometry.o: $(GEO_DEPS)

raytracer.o: $(RAY_DEPS)

drawer.o: $(DRAW_DEPS)

main: $(MAIN_DEPS)
	g++ -g $(CXXFLAGS) $(MAIN_DEPS) -O3 -o main

all : main
	./main > render.log
	python3 image.py
	gimp *png &

.PHONY: clean all

clean :
	rm *.o *.txt *.png *.log


