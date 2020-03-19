#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//BSTNode structure, to create one follow following format:
//	Node* <name> = (Node*)malloc(sizeof(Node));
//Making an array of BSTNodes is as follows:
//	Node** <arrname> = (Node**)malloc(<size of arr> * sizeof(Node*))
//		loop through and malloc each individual Node* using previous malloc

typedef enum boolin { true = 1, false = 0}boolean;

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

void heapInit();//initializes the heap with frequencies of -1
void printHeap();//prints out all the frequencies within the heap and their words
void heapify();//converts our array into a heap after we read in all the data
char* copyString(char*,char*);//copies a string from another word and properly inserts null terminator
char* combineString(char* str1, char* str2); //combines two strings and returns combined string: String result = str1+str2;
void bstInsert(char*);//inserts a word into our bst in alphabetical ordering
int bstSearch(char*);//searches for bst item and increments it's frequency if found
int compareString(char*,char*);//compares two strings and returns a negative number if the first one is lesser, and a pos number if the first one is greater 
void printBst(Node*);//prints out the BST items
void constructHeap(); //helper function to heapify our stuff
void heapify(int); //turns our array into a heap

Node* root;//our tree node, initially we store all the values in here and then into our heap
int nodeCount = 0;//amount of items in our tree

boolean rootSet = false;//indicates whether or not root is set

heapItem* heapArr;//our array of vals to start our huffman tree
int heapCount = 0; //temporary, for testing purposes

int main(int argc, char** argv) {
	/*	for testing purposes, argument checkers are commented out 	
	if (argc < 3) {
		printf("Fatal Error: expected at least 3 arguments\n");
		exit(0);
	} else if (argc > 5) {
		printf("Fatal Error: expected at most 5 arguments\n");
		exit(0);
	}
	*/

	//check flag
	char flag;
	boolean recursive = false;
	if (argv[1][1] != 'R') {
		flag = argv[1][1];
		if (argv[2][0] == '-') {
			recursive = true;
		}
		printf("flag is: %c\n", argv[1][1]);
	} else {
		printf("flag is %c\n", argv[2][1]);
		flag = argv[2][1];
		recursive = true;
	}

	if (recursive == true) { //just to check
		printf("recursive flag on\n\n");
	} else {
		printf("recursive flag off\n\n");
	}

	//call flag functions
	if (recursive) {
		if (flag == 'b') {

		} else if (flag == 'c') {

		} else if (flag == 'd') {

		}
	} else {
		if (flag == 'b') {
			char* directory = argv[2]; //all files within this directory will be indexed into HuffmanCodebook
			printf("directory is: %s\n", directory);			

			int fd;
			mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
			char* filename = "/ilab/users/grf27/cs214/asst1/fileCompressor/file.txt";
			fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, mode);

			printf("fd is: %d\n\n", fd);

			char* combined = combineString(directory, "HuffmanCodebook");
			printf("combined is: %s\n\n", combined);
		} else if (flag == 'c') {

		} else if (flag == 'd') {

		}
	}	

	bstInsert("dog\0");
	bstInsert("cat\0");
	bstInsert("dog\0");
	bstInsert("bat\0");
	bstInsert("swag\0");
	bstInsert("bat\0");
	bstInsert("dog\0");
	heapInit();
	printf("PRINTING BST\n");
	printBst(root);
	constructHeap();
	printHeap();
	return 0;
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
	int start = (nodeCount/2) - 1;
	int i;
	for (i = start; i >= 0; i--) {
		heapify(i);
	}
}

void heapify(int index) {
	int small = index;
	int left = (index * 2) + 1;
	int right = (index * 2) + 1;
	if ( left < nodeCount && heapArr[left].freq < heapArr[small].freq) {
		small = left;
	}
	
	if ( right < nodeCount && heapArr[right].freq < heapArr[small].freq) {
		small = right;
	}
	
	if (small != index) {//swaps the two values and continues to heapify
		char* tWord;
		int tFreq = heapArr[small].freq;
		copyString(tWord,heapArr[small].word);
		copyString(heapArr[small].word,heapArr[index].word);
		heapArr[small].freq = heapArr[index].freq;
		copyString(heapArr[index].word,tWord);
		heapArr[index].freq = tFreq;
		heapify(small);
	}
}

void printHeap() {
	printf("PRINTING HEAP\n");
	int i;
	for (i = 0; i < nodeCount; i++) {
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
		root->freqSet = true;
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
		temp->freqSet = true;
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

void printBst(Node* ptr) {
	if (ptr == NULL) {
		return;
	}
	printBst(ptr->left);
	printf("BST_NODE: %s: %d\n",ptr->word,ptr->freq);
	heapArr[heapCount].word = copyString(heapArr[heapCount].word,ptr->word);
	heapArr[heapCount].freq = ptr->freq;
	heapCount++;
	printBst(ptr->right);
}

char* copyString(char* to, char* from) {
	int length = strlen(from);
	to = (char*)malloc(length * sizeof(char) + 1);
	int i;
	for ( i = 0; i < length; i++) {
		to[i] = from[i];
	}
	to[length] = '\0';
	return to;
} 

char* combineString(char* str1, char* str2) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	char* result = (char*)malloc((len1+len2)*sizeof(char) + 1);
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
	result[len1 + len2] = '\0';
	return result;
}

char* substring(char* str, int index) {
	int length = strlen(str);
	char* result = (char*)malloc((length-index)*sizeof(char) + 1);
	int i;
	int j = 0;
	for ( i = index; i < length; i++) {
		result[j] = str[i];
		j++;
	}
	result[length-index] = '\0';
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





