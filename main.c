#include <stdio.h>
#include <string.h>
#include "termbox2.h"

#define MAX_LINE 1024
#define DELIMETER ","
#define COLUMN_COUNT 7

typedef enum { Transport = 0, Groceries, Restaurants, Other } Category;

Category category_from_string(const char *str) {
	if (strcmp(str, "Transport")   == 0) return Transport;
	if (strcmp(str, "Groceries")   == 0) return Groceries;
	if (strcmp(str, "Restaurants") == 0) return Restaurants;
	if (strcmp(str, "Other")     == 0) return Other;

	return Other;
}

typedef struct {
	int id;
	char *merchant;
	float amount;
	Category category;
	char *payment_method;
	int last_4;
	char *date;
} Transaction;

typedef struct {
	Transaction *transactions;
	float total_spent;
} Ledger;

int main() {

	FILE *file= fopen("ledger.csv", "r");
	if(!file) {
		perror("fopen");
		return 1;
	}

	Transaction transaction[6];
	Ledger ledger;
	ledger.total_spent = 0.0f;

	char line[MAX_LINE];

	int row_count = 0;
	while(fgets(line, sizeof(line), file)) {
		line[strcspn(line, "\n")] = '\0';

		char *token = strtok(line, DELIMETER);
		int current_column = 0;
		while(token) {
			current_column++;
			printf("%s ", token);
			switch(current_column) {
				case 1: {
									transaction[row_count].id = atoi(token);
								}
								break;
				case 2: {
									transaction[row_count].merchant = malloc(strlen(token) + 1);
									strcpy(transaction[row_count].merchant, token);
								}
								break;
				case 3: {
									transaction[row_count].amount = atof(token);
									ledger.total_spent += transaction[row_count].amount;
								}
								break;
				case 4: {
									transaction[row_count].category = category_from_string(token);
								}
								break;
				case 5: {
									transaction[row_count].payment_method = malloc(strlen(token) + 1);
									strcpy(transaction[row_count].payment_method, token); 
								}
								break;
				case 6: {
									transaction[row_count].last_4 = atoi(token);
								}
								break;
				case 7: {
									transaction[row_count].date = malloc(strlen(token) + 1);
									strcpy(transaction[row_count].date, token);
								}
								break;
			}
			token = strtok(NULL, DELIMETER);
		}
		row_count++;
		printf("\n");
	}
	ledger.transactions = transaction; 
  printf("total spent: %.1f \n", ledger.total_spent);
	printf("value of first transaction is: %.1f \n", transaction[4].amount);

	fclose(file);
	return 0;
}
