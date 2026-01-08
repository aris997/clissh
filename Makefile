clissh: clissh.c
	echo "Creating the binary"
	gcc -Wall -x c -c clissh.c -o clissh

clean:
	rm -f clissh
