.PHONY: chaos
chaos: chaos_bg.c
	gcc -O3 $^ -ljpeg -o $@

clean:
	rm -f chaos
