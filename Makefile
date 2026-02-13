clissh: clissh.c
	echo "Creating the binary"
	gcc -Wall clissh.c -o clissh

test: clissh
	bash tests/run_tests.sh

clean:
	rm -f clissh
