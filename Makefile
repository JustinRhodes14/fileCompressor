all: fileCompressor

fileCompressor: fileCompressor.c
	gcc fileCompressor.c -o fileCompressor -lm

clean: 
	rm -f fileCompressor
