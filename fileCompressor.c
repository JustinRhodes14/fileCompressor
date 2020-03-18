#include<stdio.h>
#include<stdlib.h>
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

heapItem* heapInit();//initializes the heap with frequencies of -1
void printHeap();//prints out all the frequencies within the heap and their words

Node* root;//our tree node
heapItem* heapArr;//our array of vals to start our huffman tree
int heapSize = 10;//starting heap size, will update and rehash as we continue to read in words, then heapify at the end into a min heap

int main(int argc, char** argv) {
	
	heapArr = heapInit();
	printHeap();
	
	return 0;
}

heapItem* heapInit() {

	heapItem* arr = (heapItem*)malloc(heapSize * sizeof(heapItem));
	int i;
	for (i = 0; i < heapSize; i++) {
		arr[i].freq = -1; //indicates no item is inserted
		arr[i].isTree = false;
	}
	
	return arr;
}

void printHeap() {
	int i;
	for (i = 0; i < heapSize; i++) {
		printf("%s: %d, %d\n",heapArr[i].word,heapArr[i].freq,heapArr[i].isTree);
	}	
}








