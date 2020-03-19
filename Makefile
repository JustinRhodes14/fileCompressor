all: fileCompressor

second: fileCompressor.c
	gcc fileCompressor.c -o fileCompressor

clean: 
	rm -f fileCompressor
