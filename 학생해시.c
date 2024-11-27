#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parson.h"

#define TABLE_SIZE 5000000
#define NAME_SIZE 100

typedef struct {
	int key;
	char name[NAME_SIZE];
	int phone_number;
	int late_number;
} element;

struct list {
	element item;
	struct list* link;
};

struct list* hash_table[TABLE_SIZE];

int hash_function(int key) {
	return key /* % TABLE_SIZE*/;
}

// 해시 데이터 추가
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
	ptr->item.late_number = item.late_number;

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
		printf("%d, %s, 0%d %d", node->item.key, node->item.name, node->item.phone_number, node->item.late_number);
	}
	printf("\n");
	printf("=======================================\n");
}

// 해시 데이터 수정
void hash_chain_change(int key, int change_value) {
	struct list* node;
	int hash_value = hash_function(key);
	if (hash_table[key] == NULL) {
		printf("등록된 학번이 없습니다.\n");
		return;
	}
	node = hash_table[hash_value];
	node->item.late_number = change_value;

}

// JSON 파일 읽어와서 해시에 저장
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

			data.late_number = (int)json_object_get_number(item, "dueDateOverNumber");
			printf("연체 횟수 : %d\n", data.late_number);

			hash_chain_add(data, hash_table);
			printf("\n");
		}
	}
	json_value_free(root_Value);
}

size_t json_file_find_index(int student_num, JSON_Array* find_data) {
	size_t array_size = json_array_get_count(find_data);
	for (size_t i = 0; i < array_size; i++) {
		JSON_Object* item = json_array_get_object(find_data, i);
		if (item != NULL && ((int)json_object_get_number(item, "studentNumber")) == student_num) {
			return i;
		}
	}
	return -1;
}

// 블랙리스트 추가
void json_blacklist_add(int student_num) {
	// 연체 횟수 3회 초과할 시 3달 제한
	int borrow_limit_day = 3;

	JSON_Value* root_Value = json_parse_file("blackList.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "blackListInfo");
	size_t array_size = json_array_get_count(data_Array);

	JSON_Value* blackList_data = json_value_init_object();
	JSON_Object* blackList_object = json_value_get_object(blackList_data);

	json_object_set_number(blackList_object, "studentNumber", student_num);
	json_object_set_number(blackList_object, "borrowLimitDay", borrow_limit_day);

	json_array_append_value(data_Array, blackList_data);
	json_serialize_to_file_pretty(root_Value, "blackList.json");
	json_value_free(root_Value);
}

void json_blacklist_delete(int student_num) {
	JSON_Value* root_Value = json_parse_file("blackList.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* black_Array = json_object_get_array(root_Object, "blackListInfo");
	size_t index = json_file_find_index(student_num, black_Array);

	json_array_remove(black_Array, index);

	json_serialize_to_file_pretty(root_Value, "blackList.json");
	json_value_free(root_Value);
}

// JSON 파일 업데이트
void json_file_rewriting(int student_num) {

	JSON_Value* root_Value = json_parse_file("studentData.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "studentInfo");
	size_t array_size = json_array_get_count(data_Array);

	size_t index = json_file_find_index(student_num, data_Array);
	int change_value = 0;

	JSON_Object* change = json_array_get_object(data_Array, index);
	int check_limit = (int)json_object_get_number(change, "dueDateOverCheck");

	if (change != NULL && check_limit != 1) {
		change_value = (int)json_object_get_number(change, "dueDateOverNumber");
		change_value += 1;
		json_object_set_number(change, "dueDateOverNumber", change_value);
		hash_chain_change(student_num, change_value);
	}
	if (change_value >= 3 && check_limit != 1) {
		json_blacklist_add(student_num);
		json_object_set_number(change, "dueDateOverCheck", 1);
		json_object_set_number(change, "dueDateOverNumber", 0);
	}
	else if (check_limit == 1) {
		json_blacklist_delete(student_num);
		json_object_set_number(change, "dueDateOverCheck", 0);
		hash_chain_change(student_num, 0);
	}
	json_serialize_to_file_pretty(root_Value, "studentData.json"); // Json 파일 변경점 저장, 이 코드가 있어야 JSON파일이 수정됨
	json_value_free(root_Value);
}

void insert_string_data(char* buffer, int size) {
	fgets(buffer, size, stdin);
	buffer[strcspn(buffer, "\n")] = '\0';
}

void json_data_add() {

	char newName[20] = "";
	int student_num = 0;
	int phone_num = 0;
	int due_date_over_num = 0;
	int borrow_limit = 0;


	printf("학번 : ");
	scanf_s("%d", &student_num);
	while (getchar() != '\n');

	printf("이름 : ");
	insert_string_data(newName, 20);

	printf("전화번호 : ");
	scanf_s("%d", &phone_num);
	while (getchar() != '\n');

	JSON_Value* root_Value = json_parse_file("studentData.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "studentInfo");
	size_t array_size = json_array_get_count(data_Array);

	JSON_Value* new_student_data = json_value_init_object();
	JSON_Object* new_student_object = json_value_get_object(new_student_data);

	json_object_set_number(new_student_object, "studentNumber", student_num);
	json_object_set_string(new_student_object, "name", newName);
	json_object_set_number(new_student_object, "phoneNumber", phone_num);
	json_object_set_number(new_student_object, "dueDateOverNumber", due_date_over_num);
	json_object_set_number(new_student_object, "dueDateOverCheck", borrow_limit);

	json_array_append_value(data_Array, new_student_data);
	json_serialize_to_file_pretty(root_Value, "studentData.json");
	json_value_free(root_Value);

}

void check_borrow_limit() {
	JSON_Value* root_Value = json_parse_file("blackList.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "blackListInfo");
	size_t array_size = json_array_get_count(data_Array);
	for (size_t i = 0; i < array_size; i++) {
		JSON_Object* change = json_array_get_object(data_Array, i);
		int student_num = (int)json_object_get_number(change, "studentNumber");
		if (change != NULL) {
			int limit_day = (int)json_object_get_number(change, "borrowLimitDay");
			if (limit_day == 0) {
				json_file_rewriting(student_num);
			}
			else if (limit_day > 0) {
				int limit_day = (int)json_object_get_number(change, "borrowLimitDay");
				json_object_set_number(change, "borrowLimitDay", limit_day - 1);
				hash_chain_change(student_num, limit_day);
				json_serialize_to_file_pretty(root_Value, "blackList.json"); // Json 파일 변경점 저장, 이 코드가 있어야 JSON파일이 수정됨
			}
		}

	}
	json_value_free(root_Value);
}

int main() {
	int choice;
	int student_num;


	json_file_reading();

	while (1) {
		check_borrow_limit();
		printf("선택지 : 1. 학생조회 2. 종료 3. 연체 학생 업데이트 4. 새로운 학생 추가=> ");
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
			json_file_rewriting(student_num);
			hash_chain_search(student_num);
		}
		else if (choice == 4) {
			json_data_add();
		}
		else
			printf("다시 입력하십시오\n\n");
	}
	return 0;
}