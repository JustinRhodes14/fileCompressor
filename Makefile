all: fileCompressor

fileCompressor: fileCompressor.c
	gcc fileCompressor.c -o fileCompressor -lm

clean: 
	rm ./fileCompressor;
cleanhcz:
	rm ./stuff/test1.txt.hcz; rm ./stuff/test2.txt.hcz; rm ./stuff/test3.txt.hcz; rm ./stuff/stuff2/test4.txt.hcz; rm ./stuff/stuff2/test5.txt.hcz;
