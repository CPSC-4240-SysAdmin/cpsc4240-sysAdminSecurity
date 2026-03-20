compile: 
	gcc main.c -ldialog -lncurses -o fpMod 

# For compulation on MacOs where libraries are installed via brew
mcompile:
	gcc main.c -I/opt/homebrew/include -L/opt/homebrew/lib -ldialog -lncurses -o fpMod 

run: 
	./fpMod

clean:
	rm fpMod