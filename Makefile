.PHONY: chaos
chaos: chaos_bg.o attractor.o hitmap.o
	g++ -O3 $^ -ljpeg -o $@

.PHONY: clean
clean:
	rm -f chaos *.o

%.o: %.cc
	g++ -O3 -o $@ -c $< -ljpeg
