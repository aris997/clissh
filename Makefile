clissh: clissh.c
	echo "Creating the binary"
	gcc -Wall clissh.c -o clissh

clean:
	rm -f clissh
