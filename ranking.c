#include<stdio.h>
#include<string.h>
#include<fcntl.h>

int readranking(int *, char[][100]);
void sort(int *, char[][100], int);
void printrank(int *, char[][100], int);

int main() {
	int rank[30];
	int people;
	char name[30][100];
	int i;

	people = readranking(rank, name);

	sort(rank, name, people);

	printrank(rank, name, people);

	return 0;
}

int readranking(int *rank, char name[][100]) {
	FILE* fp;
	int cnt = 0;

	close(0);
	if((fp = fopen("ranking.txt", "r")) == -1) {
		perror("ranking.txt");
		exit(1);
	}

	while(!feof(fp)) {
		fscanf(fp, "%d %s\n", rank + cnt, name[cnt]);
		cnt++;
	}

	return cnt;
}

void sort(int *rank, char name[][100], int cnt) {
	int i, j, temp;
	char ntemp[100];

	for(i = 0; i < cnt - 1; i++) {
		for(j = i + 1; j < cnt; j++) {
			if(rank[i] < rank[j]) {
				temp = rank[i];
				rank[i] = rank[j];
				rank[j] = temp;
				strcpy(ntemp, name[i]);
				strcpy(name[i], name[j]);
				strcpy(name[j], ntemp);
			}
		}
	}
}

void printrank(int *rank, char name[][100], int cnt) {
	int i;

	for(i = 0; i < cnt; i++) {
		printf("%d. %s: %d\n", i + 1, name[i], rank[i]);
	}
}
