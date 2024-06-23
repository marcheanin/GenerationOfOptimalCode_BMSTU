lab1:
	g++ -g -Wall -Wextra -std=c++14 -I`gcc -print-file-name=plugin`/include -fPIC -fno-rtti -shared src/main.cpp -o src/$@.so

test:
	gcc -fplugin=src/lab1.so -O0 src/test.c -o src/test
	rm src/test

.PHONY : clean

clean:
	rm -rf lab1.so src/test
