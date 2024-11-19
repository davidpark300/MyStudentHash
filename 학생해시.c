#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parson.h"

#define TABLE_SIZE 2599999
#define NAME_SIZE 100
#define DATA_SIZE 256
#define LATE_SIZE 20
int data_numbers = 0;

typedef struct {
	int key;
	char name[NAME_SIZE];
	int phone_number;
	char late[LATE_SIZE];
} element;

struct list {
	element item;
	struct list* link;
};

struct list* hash_table[TABLE_SIZE];

int hash_function(int key) {
	return key /* % TABLE_SIZE*/;
}

void hash_chain_add(element item, struct list* ht[]) {
	int hash_value = hash_function(item.key);
	struct list* ptr;
	struct list* node_before = NULL;
	struct list* node = ht[hash_value];
	for (; node; node_before = node, node = node->link) {
		if (node->item.key == item.key) {
			fprintf(stderr, "�̹� Ž��Ű�� ����Ǿ� ����\n");
			return;
		}
	}
	ptr = (struct list*)malloc(sizeof(struct list));
	ptr->item.key = item.key;
	strcpy(ptr->item.name, item.name);
	ptr->item.phone_number = item.phone_number;
	strcpy(ptr->item.late, item.late);

	ptr->link = NULL;

	if (node_before)
		node_before->link = ptr;
	else
		ht[hash_value] = ptr;
}

// ü�ι��� �̿��Ͽ� ���̺� ����� Ű�� Ž��
void hash_chain_search(int key) {
	struct list* node;
	int hash_value = hash_function(key);
	printf("\n=======================================\n");
	if (hash_table[hash_value] == NULL) {
		printf("��ϵ� �й��� �����ϴ�.\n");
		return;
	}
	for (node = hash_table[hash_value]; node; node = node->link) {
		printf("%d, %s, 0%d", node->item.key, node->item.name, node->item.phone_number);
	}
	printf("\n");
	printf("=======================================\n");
}



// csv ���� �о�ͼ� �ؽÿ� ����� �� �ֵ��� �� �����ϴ� �Լ�, ���
void csv_data_read() {
	FILE* fpr;
	char str[DATA_SIZE];
	element items;

	if ((fpr = fopen("studentData.csv", "r+")) == NULL) {
		printf("���� �б� ����\n");
		fclose(fpr);
		return;
	}

	fgets(str, DATA_SIZE, fpr);	// ���Ͽ��� ���ڿ��� �������� �Լ�

	char* sub = strtok(str, ",");

	while (1) {
		fgets(str, DATA_SIZE, fpr);
		sub = strtok(str, ",");
		items.key = atoi(sub);	// csv ������ �й� ����
		//printf("%d || ", atoi(sub));

		sub = strtok(NULL, ",");
		if (sub == NULL) {
			items.key = 0;
			//printf("������ ����\n");
			return;
		}
		strcpy(items.name, sub);	// csv ������ �̸� ����
		//printf("%s || ", sub);

		sub = strtok(NULL, ",");
		items.phone_number = atoi(sub);	// csv ������ �ڵ��� ��ȣ ����
		//printf("%d || ", atoi(sub));
		//printf("\n");

		sub = strtok(NULL, ",");
		strcpy(items.late, sub);

		data_numbers++;
		hash_chain_add(items, hash_table);
	}
	fclose(fpr);
}

void json_file_reading() {
	element data;

	JSON_Value* root_Value = json_parse_file("studentData.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "studentInfo");
	size_t array_size = json_array_get_count(data_Array);
	for (size_t i = 0; i < array_size; i++) {
		JSON_Object* item = json_array_get_object(data_Array, i);
		if (item != NULL) {
			data.key = (int)json_object_get_number(item, "studentNumber");
			printf("�й� : %d\n", data.key);
			strcpy(data.name, json_object_get_string(item, "name"));
			printf("�̸� : %s\n", data.name);
			data.phone_number = (int)json_object_get_number(item, "phoneNumber");
			printf("��ȭ��ȣ : %d\n", data.phone_number);

			if (json_object_get_string(item, "dueDateOverCheck")) {
				strcpy(data.late, "true");
			}
			else {
				strcpy(data.late, "false");
			}
			//strcpy(data.late, json_object_get_string(item, "dueDateOverCheck"));
			printf("��ü ���� : %s\n", data.late);
			hash_chain_add(data, hash_table);
			printf("\n");
		}
	}
	json_value_free(root_Value);
}

void json_file_writing(int student_num, char lateChek[LATE_SIZE]) {
	int k;
	JSON_Value* root_Value = json_parse_file("studentData.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "studentInfo");
	size_t array_size = json_array_get_count(data_Array);

	JSON_Object* change = json_array_get_object(data_Array, 0);
	if (change != NULL) {
		json_object_set_number(change, "studentNumber", student_num);
	}
	json_value_free(root_Value);
}

int main() {
	int choice;
	int student_num;

	while (1) {
		json_file_reading();
		printf("������ : 1. �л���ȸ 2. ���� 3. (�̿ϼ����)���� ������Ʈ=> ");
		scanf_s("%d", &choice);
		if (choice == 1) {
			printf("�й� �Է� => ");
			scanf_s("%d", &student_num);
			hash_chain_search(student_num);
		}
		else if (choice == 2)
			break;
		else if (choice == 3) {
			printf("�й� �Է� => ");
			scanf_s("%d", &student_num);
			json_file_writing(student_num, "false");
		}
		else
			printf("�ٽ� �Է��Ͻʽÿ�\n\n");
	}

	return 0;
}