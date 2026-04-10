compile: 
	gcc main.c -ldialog -lncursesw -lm -g -o permod

# For compulation on MacOs where libraries are installed via brew
mcompile:
	gcc main.c -I/opt/homebrew/include -L/opt/homebrew/lib -ldialog -lncurses -g -o permod

run: 
	./permod

clean:
	rm permod
