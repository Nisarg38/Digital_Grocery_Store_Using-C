#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>


#define CAT_LEN 30
#define ITEM_LEN 17

typedef struct item 
{

	int ItemCode;
	int NumItem;
	float ItemPrice;
	float TotSale;
	char itemName[ITEM_LEN];

	struct item* next_item;

} ITEM;

typedef struct cat
 {
	int total_items_sold;
	float total_sales;
	char cat_name[CAT_LEN];

	ITEM* items;

} Category;



//prototypes

// a Function which  adds an item to the linked list
void add_item(ITEM** head, ITEM* item);

//	the function is used to print item list to lemma specified
void print_list(FILE* file, ITEM* item_list);

//	this Function deallocates the allocated memory for each item in linked list
void deAllocate_item(ITEM* head);

//this function is used	to create a new item
ITEM* create_item(int code, char* name, float price);

// search for an item in the linked list
ITEM* search_item(ITEM* head, int code);





int main()
{

	//	Declaring variables and structures for the program
	Category inventory[8];
	int code;
	char item_name[ITEM_LEN];
	float item_price;
	int customer_id = 1;
	int quantity;
	int i,a,j;
	int total_items_sold = 0;
	float total_sales = 0.0;

	ITEM* temp;

	for (i = 0; i < 8; i++) 
	{
		inventory[i].items = NULL;
		inventory[i].total_items_sold = 0;
		inventory[i].total_sales = 0.0;
	}

	//	Use Case-01
	FILE* cat_name_file = fopen("CategoryName.dat", "r");

	while (!feof(cat_name_file)) {
		fscanf(cat_name_file, "%d\t", &code);
		if (feof(cat_name_file))
			continue;
		fgets(inventory[(code / 100) - 1].cat_name, CAT_LEN, cat_name_file);
		inventory[(code / 100) - 1].cat_name[strlen(inventory[(code / 100) - 1].cat_name) - 2] = '\0';
	}

	//	Use Case-02
	FILE* cod_name_file = fopen("CodeNamePrice.dat", "r");

	while (!feof(cod_name_file)) {
		fscanf(cod_name_file, "%d\t", &code);
		if (feof(cod_name_file))
			continue;
		fscanf(cod_name_file, "%[^\t]", item_name);
		fscanf(cod_name_file, "%f", &item_price);

		//	Adding item to linked list
		ITEM* item = create_item(code, item_name, item_price);
		add_item(&(inventory[(code / 100) - 1].items), item);
	}

	//	Use Case-03
	FILE* customer_data = fopen("DailyTransactions.dat", "r");

	ITEM* customer_list = NULL;
	ITEM* customer_item = NULL;

	while (!feof(customer_data)) 
	{
		fscanf(customer_data, "%d\t", &code);
		if (code != 0) 
		{
			fscanf(customer_data, "%d", &quantity);
			
			//	Searching an item in list
			temp = search_item(inventory[(code / 100) - 1].items, code);

			temp->NumItem = temp->NumItem + quantity;
			customer_item = create_item(code, temp->itemName, temp->ItemPrice);
			customer_item->NumItem = quantity;
			customer_item->TotSale = customer_item->ItemPrice * customer_item->NumItem;

			
			inventory[(code / 100) - 1].total_items_sold = inventory[(code / 100) - 1].total_items_sold + quantity;
			inventory[(code / 100) - 1].total_sales = inventory[(code / 100) - 1].total_sales + (customer_item->ItemPrice * customer_item->NumItem);

			//	adding item to the item's linked list
			add_item(&customer_list, customer_item);

		}
	}

	//	Use Case-04
	printf("\n ** CUSTOMER RECEIPT ** # %d \n", customer_id++);

	printf("%-10s%-20s%-10s%-10s%s\n", "Code", "Item Name", "Price", "Num Item", "Total Sales");

	customer_item = customer_list;
	
	while (customer_item) 
	{
		printf("%-10d%-20s%-10.2f%-10d%0.2f\n", customer_item->ItemCode, customer_item->itemName, customer_item->ItemPrice, customer_item->NumItem, customer_item->TotSale);
		customer_item = customer_item->next_item;
	}

	//	Use Case-05
	char cat_code[] = "000";
	char cat_file[] = "Inventory000.dat";
	for (a= 0; a < 8; a++) 
	{

		puts(" ");

		printf("CATEGORY NAME : %s\n", inventory[a].cat_name);

		cat_code[0] = a + '1';

		printf("CATEGORY CODE: %s\n", cat_code);

		printf("%-10s%-20s%-10s%s\n", "code", "Item Name", "Num Item", "Total Sales");
	
		cat_file[9] = a + '1';

		FILE* cust_file = fopen(cat_file, "w");


		print_list(stdout, inventory[a].items);

		printf("%-20s%d\n", "Total Items Sold: ", inventory[a].total_items_sold);

		printf("%-20s%0.2f\n", "Total Sales: ", inventory[a].total_sales);

		print_list(cust_file, inventory[a].items);

		//	Closing file descriptor
		fclose(cust_file);
	}


	fclose(cat_name_file);
	fclose(cod_name_file);
	fclose(customer_data);

	//	Use Case-06
	printf("\n");
	printf("DAILY SUMMARY REPORT\n");
	printf("%-10s%-20s%-20s%s\n", "code", "category name", "#items sold", "total sales");
	for (j = 0; j < 8; j++) 
	{

		total_items_sold = total_items_sold + inventory[j].total_items_sold;
		total_sales = total_sales + inventory[j].total_sales;

		cat_code[0] = j + '1';
		printf("%-10s", cat_code);
		printf("%-20s", inventory[j].cat_name);
		printf("%-20d", inventory[j].total_items_sold);
		printf("%0.2f", inventory[j].total_sales);
		printf("\n");
	}

//to print 
	printf("%-20s%10d\n", "TOTAL-Customers", customer_id - 1);

	printf("%-20s%10d\n", "TOTAL-Items Sold", total_items_sold);

	printf("%-20s%10.2f\n", "TOTAL-Sales", total_sales);

	
	for (a = 0; a < 8; a++)
	{
		deAllocate_item(inventory[a].items);
		inventory[a].items = NULL;
	}

	return 0;
}

//DEFINITIONS
void add_item(ITEM** head, ITEM* item)
 {

	if (*head == NULL)
	{
		*head = item;
	}
	else if ((*head)->ItemCode > item->ItemCode)
	{
		item->next_item = *head;
		*head = item;
	}
	else {
		ITEM* temp = *head;
		while (temp->next_item && temp->next_item->ItemCode < item->ItemCode)
		{
			temp = temp->next_item;
		}
		item->next_item = temp->next_item;
		temp->next_item = item;
	}
}


ITEM* create_item(int code, char* name, float price)
{

	ITEM* temp = (ITEM*)malloc(sizeof(ITEM));
	temp->ItemCode = code;
	strcpy(temp->itemName, name);
	temp->ItemPrice = price;
	temp->NumItem = 0;
	temp->TotSale = 0.0;

	temp->next_item = NULL;

	return temp;
}


ITEM* search_item(ITEM* head, int code) 
{

	while (head)
	{
		if (head->ItemCode == code) 
		{
			return head;
		}
		head = head->next_item;
	}
	return head;
}


void print_list(FILE* file, ITEM* item_list)
{
	while (item_list) 
	{
		item_list->TotSale = ((item_list->ItemPrice) * (item_list->NumItem));

		fprintf(file, "%-10d%-20s%-10d%0.2f\n", item_list->ItemCode, item_list->itemName, item_list->NumItem, item_list->TotSale);

		item_list = item_list->next_item;
	}
}


void deAllocate_item(ITEM* head)
{
	ITEM* next;
	while (head)
	{
		next = head->next_item;
		free(head);
		head = next;
	}
}

