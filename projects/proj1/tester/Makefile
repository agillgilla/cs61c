
all: philphix 

test: philphix test.txt replace.txt
	touch output.txt
	rm output.txt
	cat test.txt | ./philphix replace.txt > output.txt
	echo "Difference between test output and reference output"
	diff output.txt reference.txt

testgend: philphix gendtest.txt gendrep.txt
	touch output.txt
	rm output.txt
	cat gendtest.txt | ./philphix gendrep.txt > output.txt
	echo "Difference between test output and reference output"
	diff output.txt gendref.txt

testautogen: philphix
	python gendict.py
	touch output.txt
	rm output.txt
	cat newtest.txt | ./philphix newrep.txt > output.txt
	echo "Difference between test output and reference output"
	diff output.txt newref.txt
philphix: philphix.o hashtable.o
	gcc -g -Wall -o philphix philphix.o hashtable.o

philphix.o: philphix.c philphix.h hashtable.h
	gcc -g -c -Wall philphix.c

hashtable.o: hashtable.c hashtable.h
	gcc -g -c -Wall hashtable.c

clean:
	rm -f *.o philphix

testall: test testgend testautogen