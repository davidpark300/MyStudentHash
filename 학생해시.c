#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parson.h"

#define TABLE_SIZE 2599999
#define NAME_SIZE 100
#define DATA_SIZE 256

typedef struct {
	int key;
	char name[NAME_SIZE];
	int phone_number;
	int lateNumber;
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
			fprintf(stderr, "이미 탐색키가 저장되어 있음\n");
			return;
		}
	}
	ptr = (struct list*)malloc(sizeof(struct list));
	ptr->item.key = item.key;
	strcpy(ptr->item.name, item.name);
	ptr->item.phone_number = item.phone_number;
	ptr->item.lateNumber = item.lateNumber;

	ptr->link = NULL;

	if (node_before)
		node_before->link = ptr;
	else
		ht[hash_value] = ptr;
}

// 체인법을 이용하여 테이블에 저장된 키를 탐색
void hash_chain_search(int key) {
	struct list* node;
	int hash_value = hash_function(key);
	printf("\n=======================================\n");
	if (hash_table[hash_value] == NULL) {
		printf("등록된 학번이 없습니다.\n");
		return;
	}
	for (node = hash_table[hash_value]; node; node = node->link) {
		printf("%d, %s, 0%d %d", node->item.key, node->item.name, node->item.phone_number, node->item.lateNumber);
	}
	printf("\n");
	printf("=======================================\n");
}

void hash_chain_change(int key, int change_value) {
	struct list* node;
	int hash_value = hash_function(key);
	if (hash_table[key] == NULL) {
		printf("등록된 학번이 없습니다.\n");
		return;
	}
	node = hash_table[hash_value];
	node->item.lateNumber = change_value;

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
			printf("학번 : %d\n", data.key);

			strcpy(data.name, json_object_get_string(item, "name"));
			printf("이름 : %s\n", data.name);

			data.phone_number = (int)json_object_get_number(item, "phoneNumber");
			printf("전화번호 : %d\n", data.phone_number);

			data.lateNumber = json_object_get_number(item, "dueDateOverCheck");
			printf("연체 여부 : %d\n", data.lateNumber);

			hash_chain_add(data, hash_table);
			printf("\n");
		}
	}
	json_value_free(root_Value);
}

void json_file_writing(int student_num, int late_check) {

	JSON_Value* root_Value = json_parse_file("studentData.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "studentInfo");
	size_t array_size = json_array_get_count(data_Array);

	JSON_Object* change = json_array_get_object(data_Array, 0);
	if (change != NULL) {
		int change_value = json_object_get_number(change, "dueDateOverCheck");
		change_value += 1;
		json_object_set_number(change, "dueDateOverCheck", change_value);
		hash_chain_change(student_num, change_value);
	}
	json_serialize_to_file_pretty(root_Value, "studentData.json"); // Json 파일 변경점 저장, 이 코드가 있어야 JSON파일이 수정됨
	json_value_free(root_Value);
}

int main() {
	int choice;
	int student_num;

	json_file_reading();

	while (1) {
		printf("선택지 : 1. 학생조회 2. 종료 3. (미완성기능)파일 업데이트=> ");
		scanf_s("%d", &choice);
		if (choice == 1) {
			printf("학번 입력 => ");
			scanf_s("%d", &student_num);
			hash_chain_search(student_num);
		}
		else if (choice == 2)
			break;
		else if (choice == 3) {
			printf("학번 입력 => ");
			scanf_s("%d", &student_num);
			json_file_writing(student_num, 0);
			hash_chain_search(student_num);
		}
		else
			printf("다시 입력하십시오\n\n");
	}
	json_file_reading();
	return 0;
}