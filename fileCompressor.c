#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>
//BSTNode structure, to create one follow following format:
//	Node* <name> = (Node*)malloc(sizeof(Node));
//Making an array of BSTNodes is as follows:
//	Node** <arrname> = (Node**)malloc(<size of arr> * sizeof(Node*))
//		loop through and malloc each individual Node* using previous malloc
//Bugs to fix:freeing
typedef enum boolin { true = 1, false = 0}boolean;
typedef enum _fMode {BUILD = 2, COMPRESS = 1, DECOMPRESS = 0}fMode;

typedef struct bstNode {
	int freq; //If node is just the combined frequency, only init the freq
	char* word; //If node is word, init this
	struct bstNode* left;
	struct bstNode* right;
	boolean freqSet;//indicates whether or not the frequency is set
	
}Node;

typedef struct _heapItem { //need to see whether or not tree is set
	int freq;
	char* word;
	Node* tree;
	boolean isTree;// all vals will be false, unless we ourselves insert a tree
}heapItem;

typedef struct _hashNode {
	char* binary;
	char* word;
	struct _hashNode* next;
}hashNode;

typedef struct _hashTable{
	int size;
	hashNode** table;
}hashTable;

void heapInit();//initializes the heap with frequencies of -1
void constructHeap(); //helper function to heapify our stuff
void heapify(int); //turns our array into a heap
void heapify2(int);//the true heapify
void printHeap();//prints out all the frequencies within the heap and their words
heapItem poll();//returns smallest value from heap
void heapInsert(heapItem);//inserts item into heap
void buildHuff();//builds our huffman tree
void printhuffTree(Node*,int*,int,int);//stores words and their codes into huffmancodebook file
void initializeHuff();//initializes huffmantree
void bstInsert(char*);//inserts a word into our bst in alphabetical ordering
int bstSearch(char*);//searches for bst item and increments it's frequency if found
void printBst(Node*);//prints out the BST items
char* copyString(char*,char*);//copies a string from another word and properly inserts null terminator
char* combineString(char*,char*); //combines two strings and returns combined string: String result = str1+str2;
char* substring(char*,int,int); //cuts a string starting from a certain index
int compareString(char*,char*);//compares two strings and returns a negative number if the first one is lesser, and a pos number if the first one is greater 
boolean readFile(char*);//reads data from a file, used for the -b flag
void listDirectories(char*,int,char*);//lists all directories and calls readFile() on all the files, used for the -b flag
void compress(char*,char*);//compresses all the data from a .txt file and outputs a .txt.hcz file with the compressed data
void decompress(char*,char*);
int* arrInit(int*);//initializes the array that allows us to compute each huffmancode
char* printArr(int*,int);//allows for us to store the code into our codebook
void tableInit(int);//initializes our global hash table
int hashcodeBin(char*);//computes the hashcode for the binary (used for decompress)
int hashcodeWord(char*);//computes the hashcode for the word (used for compress)
void hashInsert(char*,char*,boolean);//inserts binary and word into hashtable
char* hashSearch(char*,char*,boolean);
void readHuff(int,boolean);
void storeHuff(char*,boolean);
void writeTo(int,char*);
void tableFree(int);
void treeFree(Node*);

Node* root;//our tree node, initially we store all the values in here and then into our heap
int nodeCount = 0;//amount of items in our tree
int heapSize = 0;

boolean rootSet = false;//indicates whether or not root is set

heapItem* heapArr;//our array of vals to start our huffman tree
int heapCount = 0; //temporary, for testing purposes

Node* huffmanTree;//used for build, allows us to store our hash items in the codebook

char* coding = " ";//used to  encode the binary for huffman

hashTable* table;

int hashElements = 0;//number of elements in hashTable
int hashSize = 0; //size of hash table

int main(int argc, char** argv) {
	//need to finish error checks tmrw b4 doin anything	
	char flag;
	boolean recursive = false;
	if (argc == 3) {//non-recursive
		if (strlen(argv[1]) != 2) {
			printf("Fatal Error: Invalid flags used\n");
			exit(0);
		}
		if (argv[1][0] != '-' || argv[1][1] != 'b') {
			printf("Fatal error: Invalid flags used\n");
			exit(0);
		}
		int fd = open(argv[2],O_RDONLY);
		if (fd == -1) {
			printf("Fatal Error: %s is not a valid file name or directory\n",argv[2]);
			exit(0);
		}
		close(fd);
		flag = 'b';
		recursive = false;
	} else if (argc == 4) {//recursive build, nonrecursive compress, nonrecursive decompress
		if (strlen(argv[1]) != 2) {
			printf("Fatal Error: Invalid flags used\n");
			exit(0);
		}
		if (argv[1][0] != '-' || (argv[1][1] != 'R' && argv[1][1] != 'b' && argv[1][1] != 'c' && argv[1][1] != 'd')) {
			printf("Fatal Error: Invalid flags used\n");
			exit(0);	
		}
		if (argv[1][1] == 'R') {
			if (strlen(argv[2]) != 2) {
				printf("Fatal Error: Invalid flags used\n");
				exit(0);
			}
			if (argv[2][0] != '-' || argv[2][1] != 'b') {
				printf("Fatal Error: Invalid flags used\n");
				exit(0);
			}
			int fd = open(argv[3],O_RDONLY);
			if (fd == -1) {
				printf("Fatal Error: Invalid directory\n");
				exit(0);
			}
			recursive = true;
			flag = 'b';
		} else if (argv[1][1] == 'b') {
			if (strlen(argv[2]) != 2) {
				printf("Fatal Error: Invalid flags used\n");
				exit(0);
			}
			if (argv[2][0] != '-' || argv[2][1] != 'R') {
				printf("Fatal Error: Invalid flags used\n");
				exit(0);
			}
			int fd = open(argv[3],O_RDONLY);
			if (fd == -1) {
				printf("Fatal Error: Invalid directory\n");
				exit(0);
			}
			close(fd);
			flag = 'b';
			recursive = true;
		} else if (argv[1][1] == 'c' || argv[1][1] == 'd') { 
			int fd1 = open(argv[2],O_RDONLY);
			int fd2 = open(argv[3],O_RDONLY);
			if (fd1 == -1 || fd2 == -1) {
				printf("Fatal Error: Invalid file name\n");
				exit(0);
			}
			close(fd1);
			close(fd2);
			if (argv[1][1] == 'c') {
				flag = 'c';
			} else {
				flag = 'd';
			}
			recursive = false;
		}
	} else if (argc == 5) { //recursive compress/decompress
		if (strlen(argv[1]) != 2 || strlen(argv[2]) != 2) {
			printf("Fatal Error: Invalid flags\n");
			exit(0);
		}
		if ((argv[1][1] != 'R' || argv[1][1] != 'c' || argv[1][1] != 'd') && argv[1][0] != '-') {
			printf("Fatal Error: Invalid flags\n");	
			exit(0);
		}
		if (argv[1][1] == 'R') {
			if (strlen(argv[2]) != 2) {
				printf("Fatal Error: Invalid Flags\n");
				exit(0);
			}
			if (argv[2][0] != '-' || (argv[2][1] != 'c' && argv[2][1] != 'd')) {
				printf("Fatal Error: Invalid flags\n");
				exit(0);
			}
			if (argv[2][1] == 'c') {
				flag = 'c';
			} else {
				flag = 'd';
			}
		} else if (argv[1][1] == 'c' || argv[1][1] == 'd') {
			if (argv[1][1] == 'c') {
				flag = 'c';
			} else {
				flag = 'd';
			}
			if (strlen(argv[2]) != 2) {
				printf("Fatal Error: Invalid flags\n");
				exit(0);
			}
			if (argv[2][0] != '-' || argv[2][1] != 'R') {
				printf("Fatal Error: Invalid Flags\n");
				exit(0);
			}
		}
		int fd1 = open(argv[3],O_RDONLY);
		int fd2 = open(argv[4],O_RDONLY);
		if (fd1 == -1 || fd2 == -1) {
			printf("Fatal Error: Invalid directory or codebook\n");
			exit(0);
		}
		close(fd1);
		close(fd2);
		recursive = true;
	} else {
		printf("Fatal Error: expected between 3 and 5 arguments\n");
		exit(0);
	}
	printf("flag is %c\n", flag);
	printf("recursive is %d\n",recursive);
	//call flag functions
	if (recursive) {
		DIR *d;
		if (!(d = opendir(argv[3]))) {
			printf("Fatal Error: Invalid Directory\n");
			exit(0);
		}
		if (flag == 'b') {
			listDirectories(argv[3],0,NULL);
			int fd;
			printf("location of codebook is: ./\n");
			fd = open("./HuffmanCodebook", O_WRONLY | O_CREAT | O_TRUNC,00600);

			printf("fd is: %d\n\n", fd); //returns 3 if success, -1 if failed
			
			heapInit();
			printBst(root);
			heapSize = nodeCount;
			int* codeArr = arrInit(codeArr);
			constructHeap();
			//printHeap();
			buildHuff();
			printhuffTree(heapArr[0].tree,codeArr,0,fd);
			free(codeArr);
			treeFree(root);
			treeFree(huffmanTree);
			free(heapArr);
		} else if (flag == 'c') {
			tableInit(1000);
			listDirectories(argv[3],1,argv[4]);
			tableFree(1000);
		} else if (flag == 'd') {
			tableInit(1000);
			listDirectories(argv[3],2,argv[4]);
			tableFree(1000);
		}
	} else {
		if (flag == 'b') {
			char* path = argv[2]; //all files within this directory will be indexed into HuffmanCodebook
			printf("path is: %s\n", path);
			if (strlen(path) >= 4 && compareString(substring(path,(strlen(path)-4),-1),".hcz\0") == 0) {
				printf("Error: Cannot compress: %s, the contents of hcz files cannot be built into a codebook\n",path);
				exit(0);
			}
			boolean success = readFile(path);
			if (success == false) {
				exit(0);
			}	
			int fd;
			printf("location of codebook is: ./\n\n");
			fd = open("./HuffmanCodebook", O_WRONLY | O_CREAT | O_TRUNC,00600);
			//printf("fd is: %d\n\n", fd); //returns 3 if success, -1 if failed
			
			heapInit();
			printBst(root);
			heapSize = nodeCount;
			int* codeArr = arrInit(codeArr);
			constructHeap();
			//printHeap();
			buildHuff();
			printhuffTree(heapArr[0].tree,codeArr,0,fd);
			free(codeArr);
			treeFree(root);
			treeFree(huffmanTree);
			free(heapArr);
		} else if (flag == 'c') {
			tableInit(1000);
			if (compareString(substring(argv[2],(strlen(argv[2])-4),-1),".hcz\0") == 0) {
				printf("Error: Cannot compress .hcz files as they are already compressed\n");
				exit(0);
			}
			compress(argv[2],argv[3]);
			tableFree(1000);
		} else if (flag == 'd') {
			if (strlen(argv[2]) < 4 || compareString(substring(argv[2],(strlen(argv[2]) - 4), -1),".hcz\0") != 0) {
				printf("Fatal Error: %s is an invalid file, must call decompress on .hcz files\n",argv[2]);
				exit(0);
			} 
			tableInit(1000);
			decompress(argv[2],argv[3]);
			tableFree(1000);
		}
	}
	return 0;
}
void tableInit(int size) {
	table = (hashTable*)malloc(sizeof(hashTable));
	table->size = size;
	table->table = (hashNode**)malloc(size * sizeof(hashNode*));
	int i;
	for (i = 0; i < size; i++) {
		table->table[i] = NULL;
	}
	hashSize = size;
}

void tableFree(int size) {
	int i;
	for (i = 0; i < size; i++) {
		hashNode* temp = table->table[i];
		while (temp != NULL) {
			hashNode* temp2 = temp;
			temp = temp2->next;
			free(temp2);
		}
	}
	free(table);
}

void treeFree(Node* toFree) {
	if (toFree == NULL) {
		return;	
	}
	treeFree(toFree->left);
	treeFree(toFree->right);
	free(toFree->word);
	free(toFree);
}

int hashcodeBin(char* binary) {
	int len = strlen(binary);
	long int code = 0;
	int i;
	for (i = 0; i < len; i++) {
		code += (binary[0] - 0);
	}
	return (code % hashSize);
}

int hashcodeWord(char* word) {
	int len = strlen(word);
	int code = 0;
	int i;
	for (i = 0; i < len; i++) {
		code += (word[0] - 0);
	}
	return (code % hashSize);
}

void hashInsert(char* word, char* binary,boolean compress) {
	int index = -1;
	if (compress == true) {
		index = hashcodeWord(word);
	} else {
		index = hashcodeBin(binary);
	}
	if (index == -1) {
		printf("error in hashInsert\n");
		exit(0);	
	}
	hashNode* temp = table->table[index];
	hashNode* toInsert = (hashNode*)malloc(sizeof(hashNode));
	hashNode* temp2 = temp;
	while (temp2) {
		temp2 = temp2->next;
	}
	
	toInsert->word = copyString(toInsert->word,word);
	toInsert->binary = copyString(toInsert->binary,binary);
	toInsert->next = temp;
	table->table[index] = toInsert;

}

char* hashSearch(char* word, char* binary,boolean compress) {
	int index = -1;
	if (compress == true) {
		index = hashcodeWord(word);
		hashNode* temp = table->table[index];
		hashNode* temp2 = temp;
		while (temp2) {
			if (compareString(temp2->word,word) == 0) {
				return temp2->binary;
			}
			temp2 = temp2->next;
		}
	} else {
		index = hashcodeBin(binary);
		hashNode* temp = table->table[index];
		hashNode* temp2 = temp;
		while (temp2) {
			if (compareString(temp2->binary,binary) == 0) {
				return temp2->word;
			}
			temp2 = temp2->next;
		}
	}
	return NULL;//not found
	
}

int* arrInit(int* arr) {
	arr = (int*)malloc(nodeCount * sizeof(int));
	int i;
	for (i = 0; i < nodeCount; i++) {
		arr[i] = -1;
	}
	return arr;
}

char* printArr(int* arr,int index) {
	int i;
	char* codeWord = (char*)malloc(index * sizeof(char) + 1);
	for (i = 0; i < index;++i) {
		if (arr[i] == 0) {
			codeWord[i] = '0';
		} else {
			codeWord[i] = '1';
		}
	}	
	codeWord[index] = '\0';
	return codeWord;
}

void buildHuff() {
	while (heapSize > 1) {
		int sum = 0;
		heapItem item1 = poll();
		heapItem item2 = poll();
		sum = item1.freq + item2.freq;
		Node* tree = (Node*)malloc(sizeof(Node));
		tree->freq = sum;
		tree->freqSet = true;
		tree->word = NULL;
		if (item1.isTree && item2.isTree) {
			tree->left = item1.tree;
			tree->right = item2.tree;
		} else if (item1.isTree) {
			Node* t2 = (Node*)malloc(sizeof(Node));
			t2->freqSet = false;
			t2->word = copyString(t2->word,item2.word);
			t2->freq = item2.freq;
			t2->left = NULL;
			t2->right = NULL;
			tree->left = item1.tree;
			tree->right = t2;
		} else if (item2.isTree) {
			Node* t1 = (Node*)malloc(sizeof(Node));
			t1->freqSet = false;
			t1->word = copyString(t1->word,item1.word);
			t1->freq = item1.freq;
			t1->left = NULL;
			t1->right = NULL;
			tree->left = t1;
			tree->right = item2.tree;
		} else {
			Node* t1 = (Node*)malloc(sizeof(Node));
			t1->word = copyString(t1->word,item1.word);
			t1->freqSet = false;
			t1->freq = item1.freq;
			t1->left = NULL;
			t1->right = NULL;
			Node* t2 = (Node*)malloc(sizeof(Node));
			t2->freqSet = false;
			t2->word = copyString(t2->word,item2.word);
			t2->freq = item2.freq;
			t2->left = NULL;
			t2->right = NULL;
			tree->left = t1;
			tree->right = t2;
		}
		heapItem toInsert = heapArr[heapSize];
		toInsert.freq = sum;
		toInsert.word = "tree\0";
		toInsert.tree = tree;
		toInsert.isTree = true;
		heapInsert(toInsert);
	}
	
}
void compress(char* toCompress,char* huffBook) {
	int codebook;
	codebook = open(huffBook, O_RDONLY);
	if (codebook == -1 || compareString("./HuffmanCodebook\0",huffBook) != 0) {
		printf("Fatal Error: Invalid codebook name, should be of type './HuffmanCodebook'\n");
		exit(0);	
	}
	readHuff(codebook,true);
	int compressed;
	char* newFile = combineString(toCompress,".hcz");
	compressed = open(newFile,O_WRONLY | O_CREAT | O_TRUNC,00600);
	printf("file created %s\n",newFile);
	
	int fileParse = open(toCompress, O_RDONLY);
	int status = 1;
	int bytesRead = 0;
	char tabDelim = '\t';
	char spaceDelim = ' ';
	char lineDelim = '\n';
	char* holder;
	boolean moreStuff = false;
	boolean first = true;
	while (status > 0) {
		char buffer[101];
		memset(buffer,'\0',101);
		int readIn = 0;
		do {
			status = read(fileParse,buffer,100-readIn);
			if (status == 0) {
				break;
			}
			readIn+= status;
		}while(readIn < 100);
		if (first == true && readIn == 0) {
			printf("Warning: Empty file\n");
			return;
		}
		first = false;
		int end = 0;
		int start = 0;
		while (end < 100) {
			char* temp;
			if (buffer[end] == tabDelim || buffer[end] == spaceDelim || buffer[end] == lineDelim) {
				temp = substring(buffer,start,end);
				if (moreStuff) {
					if (strlen(holder) != 0) {
						holder = combineString(holder,temp);
						char* binInsert = hashSearch(holder,NULL,true);
						if (binInsert == NULL) {
							printf("Error: Word not present in current codebook, rebuild the current codebook according to which files you are trying to compress and rerun\n");
							exit(0);
						}
						writeTo(compressed,binInsert);
						moreStuff = false;
					}
				} else {
					if (strlen(temp) != 0) {
						char* binInsert = hashSearch(temp,NULL,true);
						if (binInsert == NULL) {
							printf("Error: Word not present in current codebook, rebuild the current codebook according to which files you are trying to compress and rerun\n");
							exit(0);
						}
						writeTo(compressed,binInsert);
					}
				}
				free(temp);
				char* binInsert2;
				if (buffer[end] == tabDelim) {
					binInsert2 = hashSearch("\t\0",NULL,true);//one tab represents tab literal
				} else if (buffer[end] == spaceDelim) {
					binInsert2 = hashSearch("\t\t\0",NULL,true);//two tabs represents space
				} else if (buffer[end] == lineDelim) {
					binInsert2 = hashSearch("\t\t\t\0",NULL,true);//three tabs represents line
				}
				if (binInsert2 == NULL) {
					printf("Error: Word not present in current codebook, rebuild the current codebook according to which files you are trying to compress and rerun\n");
					exit(0);
				}
				writeTo(compressed,binInsert2);
				start = end+1;
			}
			if (end == 99 && (buffer[end] != spaceDelim && buffer[end] != tabDelim && buffer[end] != lineDelim)) {
				if (moreStuff) {
				holder = combineString(holder,buffer);
				} else {
				holder = substring(buffer,start,-1);
				}
				moreStuff = true;
			}
			if (buffer[end] == '\0') {
				break;	
			}
			end++;
		}
		
	}
	free(newFile);
	close(fileParse);
}

void writeTo(int fd,char* word) {
	int bytesWritten = 0;
	int bytestoWrite = strlen(word);
	while (bytesWritten < bytestoWrite) {
		bytesWritten = write(fd,word,bytestoWrite-bytesWritten);
	}
}
void decompress(char* toDecompress,char* huffBook) {
	int codebook;
	codebook = open(huffBook, O_RDONLY);
	if (codebook == -1 || compareString("./HuffmanCodebook\0",huffBook) != 0) {
		printf("Fatal Error: Invalid codebook used, should be of './HuffmanCodebook'\n");
		exit(0);
	}
	readHuff(codebook,false);
	int decompressed;
	char* output = substring(toDecompress,0,(strlen(toDecompress)-4));
	decompressed = open(output,O_WRONLY | O_CREAT | O_TRUNC,00600);
	printf("file created %s\n",output);
	
	int fileParse = open(toDecompress, O_RDONLY);
	int status = 1;
	int bytesRead = 0;
	char tabDelim = '\t';
	char spaceDelim = ' ';
	char lineDelim = '\n';
	char* holder;
	boolean moreStuff = false;
	boolean first = true;
	char* fileText = "";
	while (status > 0) {
		char buffer[101];
		memset(buffer,'\0',101);
		int readIn = 0;
		do {
			status = read(fileParse,buffer,100-readIn);
			if (status == 0) {
				break;
			}
			readIn+= status;
		}while (readIn < 100);
		if (first == true && readIn == 0) {
			printf("Warning: Empty file\n");
			return;
		}
		first == false;
		fileText = combineString(fileText,buffer);
	}
	int length = strlen(fileText);
	int start = 0;
	int end = 0;
	boolean inserted = true;
	while (end < length) {
		char* temp = substring(fileText,start,end);
		inserted = false;
		char* word = hashSearch(NULL,temp,false);
		if (word != NULL) {
			if (compareString(word,"\t\0") == 0) { //one tab represents tab literal
				writeTo(decompressed,"\t\0");
			} else if (compareString(word,"\t\t\0") == 0) { //two tabs represents space
				writeTo(decompressed," \0");
			} else if (compareString(word,"\t\t\t\0") == 0) { //three tabs represents new line
				writeTo(decompressed,"\n\0");
			} else {
				writeTo(decompressed,word);
			}
			start = end;
			inserted = true;
		}
		end++;
	}	
	if (inserted == false) {
		printf("Fatal Error: Could not decompress: %s the contents of the file, one or more of the words was not present in the codebook\n",toDecompress);
		return;
	}
	close(fileParse);
}
void readHuff(int codebook,boolean compBool) {
	int status = 1;
	int bytesRead = 0;
	char lineDelim = '\n';
	char* holder;
	boolean moreStuff = false;
	boolean first = true;
	while (status > 0) {
		char buffer[101];
		memset(buffer,'\0',101);
		int readIn = 0;
		do {
			status = read(codebook,buffer,100-readIn);
			if (status == 0) {
				break;
			}
			readIn+= status;
		}while(readIn < 100);
		int end = 0;
		int start = 0;
		while (end < 100) {
			char* temp;
			if (buffer[end] == lineDelim) {
				temp = substring(buffer,start,end);
				if (moreStuff) {
					holder = combineString(holder,temp);
					moreStuff = false;
					storeHuff(holder,compBool);
				} else {
					storeHuff(temp,compBool);
				}
				start = end + 1;
				free(temp);
			} 
			if (end == 99) {
				if (moreStuff) {
				holder = combineString(holder,buffer);
				} else {
				holder = substring(buffer,start,-1);
				}
				moreStuff = true;
			}
			if (buffer[end] == '\0') {
				break;	
			}
			end++;
		}
		
	}
	close(codebook);
}

void storeHuff(char* line,boolean comp) {
	int length = strlen(line);
	int start = 0;
	int end = 0;
	char* binary;//binary
	char* word;//word
	while (line[end] != '\t') {
		end++;
	}
	binary = substring(line,start,end);
	word = substring(line,end+1,-1);
	hashInsert(word,binary,comp);
	hashSearch(word,binary,comp);
	free(binary);
	free(word);
}

heapItem poll() {
	if (heapSize == 1) {
		heapSize--;
		return heapArr[0];
	}
	
	heapItem parent = heapArr[0];
	
	heapArr[0] = heapArr[heapSize-1];
	
	heapSize--;
	heapify2(0);
	
	return parent;

}

void heapInsert(heapItem item) {
	heapSize++;
	int index = heapSize - 1;
	int parent = (index - 1)/2;
	heapArr[index] = item;

	while (index != 0 && heapArr[parent].freq > heapArr[index].freq) {
		heapItem temp = heapArr[index];
		heapArr[index] = heapArr[parent];
		heapArr[parent] = temp;
		index = parent;
	}	
}

void heapify2(int i) {
	int left = (2 * i) + 1;
	int right = (2 * i) + 2;
	int small = i;
	if (left < heapSize && heapArr[left].freq < heapArr[i].freq) {
		small = left;
	}	
	
	if (right < heapSize && heapArr[right].freq < heapArr[small].freq) {
		small = right;
	}

	if (small != i) {
		heapItem temp = heapArr[i];
		heapArr[i] = heapArr[small];
		heapArr[small] = temp;
		heapify2(small);
	}
}
void listDirectories(char* path,int mode,char* huffBook) {
		
	DIR *d;
	struct dirent *dir;
	if (!(d = opendir(path))) {
		return;
	}
	while ((dir = readdir(d)) != NULL) {
		if (dir->d_type == DT_DIR) {
			if (compareString(dir->d_name,".") == 0 || compareString(dir->d_name,"..") == 0) {
				continue;
			}
			//printf("folder: %s\n",dir->d_name);
			char* temp = combineString(path,"/");
			temp = combineString(temp,dir->d_name);
			//printf("temp is: %s\n", temp);
			listDirectories(temp,mode,huffBook);
		} else {
			
			char* temp = combineString(path,"/");
			temp = combineString(temp,dir->d_name);
			if (mode == 0) { //build
				if (strlen(temp) >= 4 && compareString(substring(temp,(strlen(temp)-4),-1),".hcz\0") == 0) {
					printf("Warning: Cannot compress: %s, the contents of hcz files cannot be built into a codebook\n",temp);
					continue;
				}
				boolean success = readFile(temp);
				if (success == false) {
					continue;
				}
			} else if (mode == 1) {//compress
				if (compareString(substring(temp,(strlen(temp)-4),-1),".hcz\0") == 0) {
					printf("Warning: Cannot compress: %s, .hcz files are already compressed\n",temp);
					continue;
				}
				compress(temp,huffBook);
			} else if (mode == 2) {//decompress
				if (compareString(substring(temp,(strlen(temp)-4),-1),".hcz\0") != 0 || strlen(temp) < 4) {
					printf("Warning: Cannot decompress: %s, can only decompress .hcz files\n",temp);
					continue;
				}
				decompress(temp,huffBook);
			}
			printf("file read: %s\n",dir->d_name);
		}
	}
	closedir(d);
}

boolean readFile(char* fileName) {
	int fileParse = open(fileName, O_RDONLY);
	int status = 1;
	int bytesRead = 0;
	char tabDelim = '\t';
	char spaceDelim = ' ';
	char lineDelim = '\n';
	char* holder = NULL;
	boolean moreStuff = false;
	boolean first = true;
	while (status > 0) {
		char buffer[101];
		memset(buffer,'\0',101);
		int readIn = 0;
		do {
			status = read(fileParse,buffer,100-readIn);
			if (status == 0) {
				break;
			}
			readIn+= status;
		}while(readIn < 100);
		if (first == true && readIn == 0) {
			printf("Warning: Empty file\n");
			return false;
		}
		first = false;
		int end = 0;
		int start = 0;
		while (end < 100) {
			char* temp;
			if (buffer[end] == tabDelim || buffer[end] == spaceDelim || buffer[end] == lineDelim) {
				temp = substring(buffer,start,end);
				if (buffer[end] == tabDelim) {
					bstInsert("\t\0");//one tab represents tab
				} else if (buffer[end] == spaceDelim) {
					bstInsert("\t\t\0");//two tabs represents space
				} else if (buffer[end] == lineDelim) {
					bstInsert("\t\t\t\0");//three tabs represents newLine
				}
				if (moreStuff) {
					holder = combineString(holder,temp);
					if (strlen(holder) != 0) {
						bstInsert(holder);
						moreStuff = false;
					}
				} else {
					if (strlen(temp) != 0) {
						bstInsert(temp);
						free(temp);
					}
				}
				start = end+1;
			}
			if (end == 99 && (buffer[99] != tabDelim || buffer[99] != spaceDelim || buffer[99] != lineDelim)) {
				if (moreStuff) {
				holder = combineString(holder,buffer);
				} else {
				holder = substring(buffer,start,-1);
				}
				moreStuff = true;
			}
			if (buffer[end] == '\0') {
				break;	
			}
			end++;
		}	
	}	
	if (holder != NULL) {
		free(holder);
	}
	close(fileParse);
	return true;	
}

void heapInit() {
	heapArr = (heapItem*)malloc(nodeCount * sizeof(heapItem));
	int i;
	for (i = 0; i < nodeCount; i++) {
		heapArr[i].freq = -1; //indicates no item is inserted
		heapArr[i].isTree = false;
	}
}
	

void constructHeap() {
	int start = (heapSize/2) - 1;
	int i;
	for (i = start; i >= 0; i--) {
		heapify(i);
	}
}

void heapify(int index) {
	int small = index;
	int left = (index * 2) + 1;
	int right = (index * 2) + 2;
	if ( left < heapSize && heapArr[left].freq < heapArr[small].freq) {
		small = left;
	}
	
	if ( right < heapSize && heapArr[right].freq < heapArr[small].freq) {
		small = right;
	}
	
	if (small != index) {//swaps the two values and continues to heapify
		char* tWord;
		int tFreq = heapArr[small].freq;
		tWord =	copyString(tWord,heapArr[small].word);
		heapArr[small].word = copyString(heapArr[small].word,heapArr[index].word);
		heapArr[small].freq = heapArr[index].freq;
		heapArr[index].word = copyString(heapArr[index].word,tWord);
		heapArr[index].freq = tFreq;
		heapify(small);
		free(tWord);
	}
}

void printHeap() {
	printf("PRINTING HEAP\n");
	int i;
	for (i = 0; i < heapSize; i++) {
		printf("%s: %d, %d\n",heapArr[i].word,heapArr[i].freq,heapArr[i].isTree);
	}	

}

void bstInsert(char* word) {
	if (rootSet == false) {
		root = (Node*)malloc(sizeof(Node));
		root->freq = 1;
		root->word = copyString(root->word,word);
		root->left = NULL;
		root->right = NULL;
		root->freqSet = false;
		rootSet = true;
		nodeCount++;
	} else if (bstSearch(word) == 1) {//1 indicates successfully found and incremented frequency
		return;
	} else {
		Node* prev;
		Node* ptr = root;
		boolean left = false;
		while (ptr != NULL) {
			if (compareString(ptr->word, word) > 0) {
				prev = ptr;
				ptr = ptr->left;
				left = true;
			} else if (compareString(ptr->word,word) < 0) {
				prev = ptr;
				ptr = ptr->right;
				left = false;
			}
		}
		Node* temp = (Node*)malloc(sizeof(Node));
		temp->freq = 1;
		temp->word = copyString(temp->word,word);
		temp->left = NULL;
		temp->right = NULL;
		temp->freqSet = false;
		if (left == true) {
			prev->left = temp;
		} else {
			prev->right = temp;
		}	
		nodeCount++;
	}	
}

int bstSearch(char* word) {
	Node* ptr = root;
	while (ptr != NULL) {
		if (compareString(ptr->word,word) < 0) {
			ptr = ptr->right;
		} else if (compareString(ptr->word,word) > 0) {
			ptr = ptr->left;
		} else {
			ptr->freq+= 1;
			return 1;
		}
	}
	return -1;//not found
}
void printhuffTree(Node* ptr,int* codeArr,int index,int fd) {
	if (ptr->left) {
		codeArr[index] = 0;
		printhuffTree(ptr->left,codeArr,index+1,fd);
	}

	if (ptr->right) {
		codeArr[index] = 1;
		printhuffTree(ptr->right,codeArr,index+1,fd);
	}

	if (!(ptr->left) && !(ptr->right)) {
		char* codeWord;
		codeWord = printArr(codeArr,index);
		int bytesWritten = 0;
		//char* combinedWord = combineString(codeWord,"\t");
		//combinedWord = combineString(combinedWord,ptr->word);
		writeTo(fd,codeWord);
		writeTo(fd,"\t\0");
		writeTo(fd,ptr->word);
		writeTo(fd,"\n\0");
		//int bytestoWrite = strlen(combinedWord);
		//while (bytesWritten < bytestoWrite) {
		//	bytesWritten = write(fd,combinedWord,bytestoWrite-bytesWritten);
		//}
		//int t = write(fd,"\n",1);
	}
}
void printBst(Node* ptr) {
	if (ptr == NULL) {
		return;
	}
	printBst(ptr->left);
	heapArr[heapCount].word = copyString(heapArr[heapCount].word,ptr->word);
	heapArr[heapCount].freq = ptr->freq;
	heapCount++;
	printBst(ptr->right);
}

char* copyString(char* to, char* from) {
	int length = strlen(from);
	to = (char*)malloc(length * sizeof(char) + 1);
	memset(to,'\0',(length+1));
	int i;
	for ( i = 0; i < length; i++) {
		to[i] = from[i];
	}
	return to;
} 

char* combineString(char* str1, char* str2) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	char* result = (char*)malloc((len1+len2)*sizeof(char) + 1);
	memset(result,'\0',(len1+len2+1));
	int i;
	int j = 0;
	for ( i = 0; i < len1; i++) {
		result[i] = str1[i];
		j++;
	}
	for ( i = 0; i < len2; i++) {
		result[j] = str2[i];
		j++;
	}
	return result;
}

char* substring(char* str, int start, int end) {
	char* result;
	if (end == -1) {
		int length = strlen(str);
		result = (char*)malloc((length-start)*sizeof(char) + 1);
		memset(result,'\0',(length-start + 1));
		int i;
		int j = 0;
		for ( i = start; i < length; i++) {
			result[j] = str[i];
			j++;
		}
	} else {
		result = (char*)malloc((end-start)*sizeof(char) + 1);
		memset(result,'\0',(end-start + 1));
		int i;
		int j = 0;
		for ( i = start; i < end; i++) {
			result[j] = str[i];
			j++;
		}	
	}
	return result;
}

int compareString(char* str1, char* str2) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int shorter = 0;
	int len = len1;
	if (len1 > len2) {
		shorter = 1;//first one is shorter
		len = len1;
	} else if (len2 < len1) {
		shorter = 2;//second one is shorter
		len = len2;
	} else {
		shorter = 0;//equal length
		len = len1;
	}
	
	int i;
	for ( i = 0; i < len; i++) {
		if (str1[i] != str2[i]) {
			return ((int)str1[i] - (int)str2[i]);//negative if str1 is lesser, positive if str1 is greater
		}	
	}
	if (len1 < len2) {
		return -1;
	} else if (len2 < len1) {
		return 1;
	}
	return 0;//equal strings
}





