##
# 
#
# @file
# @version 0.1


raylib: test.c
	gcc test.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o raylib

clean:
	rm -f raylib

run:
	./raylib

# end
