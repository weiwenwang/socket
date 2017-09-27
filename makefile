CC=g++
exe=main

obj=main.o

$(exe):$(obj)
	$(CC) -Wall -o $(exe) $(obj)

# main.o:main.cpp
# 	$(CC) -Wall -c $<

# person.o:person.cpp
# 	$(CC) -Wall -c $<

# dog.o:dog.cpp
# 	$(CC) -Wall -c $<

clean:
	rm -rf *.o main
