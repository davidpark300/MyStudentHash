#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parson.h"
#include <time.h>
#include <math.h>

#define TABLE_SIZE 5000000
#define NAME_SIZE 100
#define MAX_QUEUE_SIZE 5

typedef struct {
	char noteBookName[NAME_SIZE];
	int brokenNumber;
} noteBook;

typedef struct {
	noteBook data[MAX_QUEUE_SIZE];
	int front, rear;
} QueueType;

void print_queue_state(QueueType* q) {
	printf("Queue State - front: %d, rear: %d\n", q->front, q->rear);
	for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
		printf("[%d]: %s, %d\n", i, q->data[i].noteBookName, q->data[i].brokenNumber);
	}
}

void init_queue(QueueType* q) {
	q->front = 0;
	q->rear = 0;
}

int is_empty(QueueType* q) {
	return (q->front == q->rear);
}

int is_full(QueueType* q) {
	return ((q->rear + 1) % MAX_QUEUE_SIZE == q->front);
}

void enqueue(QueueType* q, noteBook noteBook) {
	if (is_full(q)) {
		printf("ť�� ��ȭ�����Դϴ�");
		return;
	}
	q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
	q->data[q->rear] = noteBook;
	//printf("Enqueued: %s, %d\n", noteBook.noteBookName, noteBook.brokenNumber);
	//print_queue_state(q);

}

noteBook dequeue(QueueType* q) {
	noteBook emptyNoteBook = { "", 0 };
	if (is_empty(q)) {
		//printf("ť�� ��������Դϴ�");
		return emptyNoteBook;
	}
	q->front = (q->front + 1) % MAX_QUEUE_SIZE;
	//printf("Dequeued: %s, %d\n", q->data[q->front].noteBookName, q->data[q->front].brokenNumber);
	//print_queue_state(q);
	return q->data[q->front];
}

typedef struct {
	int key;
	char name[NAME_SIZE];
	int broken;
	int day;
} borrow;

struct borrowList {
	borrow item;
	struct borrowList* link;
};

struct borrowList* hash_table_borrow[TABLE_SIZE];


typedef struct {
	int key;
	char name[NAME_SIZE];
	int phone_number;
	int late_number;
} student;

struct studentList {
	student item;
	struct studentList* link;
};

struct studentList* hash_table_student[TABLE_SIZE];

void insert_string_data(char* buffer, int size) {
	fgets(buffer, size, stdin);
	buffer[strcspn(buffer, "\n")] = '\0';
}

int hash_function(int key) {
	return key /* % TABLE_SIZE*/;
}

// �ؽ� ������ �߰�
void hash_chain_borrow_add(borrow item, struct borrowList* ht[]) {
	int hash_value = hash_function(item.key);
	struct borrowList* ptr;
	struct borrowList* node_before = NULL;
	struct borrowList* node = ht[hash_value];
	for (; node; node_before = node, node = node->link) {
		if (node->item.key == item.key) {
			fprintf(stderr, "�̹� Ž��Ű�� ����Ǿ� ����\n");
			return;
		}
	}
	ptr = (struct borrowList*)malloc(sizeof(struct borrowList));
	ptr->item.key = item.key;
	strcpy(ptr->item.name, item.name);
	ptr->item.broken = item.broken;
	ptr->item.day = item.day;

	ptr->link = NULL;

	if (node_before)
		node_before->link = ptr;
	else
		ht[hash_value] = ptr;
}

// ü�ι��� �̿��Ͽ� ���̺� ����� Ű�� Ž��
void hash_chain_borrow_search(int key) {
	struct borrowList* node;
	int hash_value = hash_function(key);
	printf("=======================================\n");
	if (hash_table_borrow[hash_value] == NULL) {
		printf("��ϵ� �й��� �����ϴ�.\n");
		printf("=======================================\n");
		return;
	}
	for (node = hash_table_borrow[hash_value]; node; node = node->link) {
		printf("�й� : %d / �뿩 ��Ʈ�� : %s / �ļ� ��ġ : %d", node->item.key, node->item.name, node->item.broken);
	}
	printf("\n");
	printf("=======================================\n\n");
}

// �ؽ� ������ �߰�
void hash_chain_student_add(student item, struct studentList* ht[]) {
	int hash_value = hash_function(item.key);
	struct studentList* ptr;
	struct studentList* node_before = NULL;
	struct studentList* node = ht[hash_value];
	for (; node; node_before = node, node = node->link) {
		if (node->item.key == item.key) {
			fprintf(stderr, "�̹� Ž��Ű�� ����Ǿ� ����\n");
			return;
		}
	}
	ptr = (struct studentList*)malloc(sizeof(struct studentList));
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

// ü�ι��� �̿��Ͽ� ���̺� ����� Ű�� Ž��
void hash_chain_student_search(int key) {
	struct studentList* node;
	int hash_value = hash_function(key);
	printf("\n=======================================\n");
	if (hash_table_student[hash_value] == NULL) {
		printf("��ϵ� �й��� �����ϴ�.\n");
		printf("=======================================\n\n");
		return;
	}
	for (node = hash_table_student[hash_value]; node; node = node->link) {
		printf("�й� : %d / �̸� : %s ��ȭ��ȣ : 0%d / ��ü Ƚ�� : %d", node->item.key, node->item.name, node->item.phone_number, node->item.late_number);
	}
	printf("\n");
	printf("=======================================\n\n");
}

// �ؽ� ������ ����
void hash_chain_student_change(int key, int change_value) {
	struct studentList* node;
	int hash_value = hash_function(key);
	if (hash_table_student[key] == NULL) {
		printf("��ϵ� �й��� �����ϴ�.\n");
		return;
	}
	node = hash_table_student[hash_value];
	node->item.late_number = change_value;

}

// JSON ���� �о�ͼ� �ؽÿ� ����
void json_file_student_reading() {
	student data;

	JSON_Value* root_Value = json_parse_file("studentData.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "studentInfo");
	size_t array_size = json_array_get_count(data_Array);
	for (size_t i = 0; i < array_size; i++) {
		JSON_Object* item = json_array_get_object(data_Array, i);
		if (item != NULL) {
			data.key = (int)json_object_get_number(item, "studentNumber");
			//printf("�й� : %d\n", data.key);

			strcpy(data.name, json_object_get_string(item, "name"));
			//printf("�̸� : %s\n", data.name);

			data.phone_number = (int)json_object_get_number(item, "phoneNumber");
			//printf("��ȭ��ȣ : %d\n", data.phone_number);

			data.late_number = (int)json_object_get_number(item, "dueDateOverNumber");
			//printf("��ü Ƚ�� : %d\n", data.late_number);

			hash_chain_student_add(data, hash_table_student);
			//printf("\n");
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

// ������Ʈ �߰�
void json_blacklist_add(int student_num, int current_day) {
	// ��ü Ƚ�� 3ȸ �ʰ��� �� 3��(90��) ����
	// �׽�Ʈ �Ҷ��� 3�Ϸ� �����Ͽ� �ٷ� Ȯ�ΰ���
	int borrow_limit_day = 3; // 90

	JSON_Value* root_Value = json_parse_file("blackList.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "blackListInfo");
	size_t array_size = json_array_get_count(data_Array);

	JSON_Value* blackList_data = json_value_init_object();
	JSON_Object* blackList_object = json_value_get_object(blackList_data);

	json_object_set_number(blackList_object, "studentNumber", student_num);
	json_object_set_number(blackList_object, "limitStartDay", current_day);
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

// JSON ���� ������Ʈ
void json_file_rewriting(int student_num) {

	time_t timer;
	struct tm* ct;
	timer = time(NULL);
	ct = localtime(&timer);

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
		hash_chain_student_change(student_num, change_value);
	}
	if (change_value >= 3 && check_limit != 1) {
		json_blacklist_add(student_num, ct->tm_yday);
		json_object_set_number(change, "dueDateOverCheck", 1);
		json_object_set_number(change, "dueDateOverNumber", 0);
	}
	else if (check_limit == 1) {
		json_blacklist_delete(student_num);
		json_object_set_number(change, "dueDateOverCheck", 0);
		hash_chain_student_change(student_num, 0);
	}
	json_serialize_to_file_pretty(root_Value, "studentData.json"); // Json ���� ������ ����, �� �ڵ尡 �־�� JSON������ ������
	json_value_free(root_Value);
}

void json_data_add() {

	char newName[20] = "";
	int student_num = 0;
	int phone_num = 0;
	int due_date_over_num = 0;
	int borrow_limit = 0;

	student data;

	printf("�й� : ");
	scanf_s("%d", &student_num);
	while (getchar() != '\n');
	data.key = student_num;

	printf("�̸� : ");
	insert_string_data(newName, 20);
	strcpy(data.name, newName);

	printf("��ȭ��ȣ : ");
	scanf_s("%d", &phone_num);
	while (getchar() != '\n');
	data.phone_number = phone_num;

	data.late_number = due_date_over_num;

	hash_chain_student_add(data, hash_table_student);

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

void check_borrow_limit(int current_day) {
	JSON_Value* root_Value = json_parse_file("blackList.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "blackListInfo");
	size_t array_size = json_array_get_count(data_Array);
	for (size_t i = 0; i < array_size; i++) {
		JSON_Object* change = json_array_get_object(data_Array, i);
		int student_num = (int)json_object_get_number(change, "studentNumber");
		if (change != NULL) {
			int limit_day = (int)json_object_get_number(change, "borrowLimitDay");
			int limitStartDay = (int)json_object_get_number(change, "limitStartDay");
			if (limit_day == 0) {
				json_file_rewriting(student_num);
			}
			else if ((limitStartDay + 1) <= /*current_day*/ 500) { // 500�� �Ϸ簡 ������ �ʾƼ� ������ �׽�Ʈ�ϱ� ���� ���Ƿ� �Է��� ����, ������ current_day
				int limit_day = (int)json_object_get_number(change, "borrowLimitDay");
				json_object_set_number(change, "borrowLimitDay", limit_day - 1);
				hash_chain_student_change(student_num, limit_day);
				json_serialize_to_file_pretty(root_Value, "blackList.json"); // Json ���� ������ ����, �� �ڵ尡 �־�� JSON������ ������
			}
		}

	}
	json_value_free(root_Value);
}

// JSON ���� �о�ͼ� �ؽÿ� ����
void json_file_noteBook_reading(QueueType* queue) {
	noteBook data;

	JSON_Value* root_Value = json_parse_file("noteBook.json");
	JSON_Object* root_Object = json_value_get_object(root_Value);

	JSON_Array* data_Array = json_object_get_array(root_Object, "noteBookInfo");
	size_t array_size = json_array_get_count(data_Array);
	for (size_t i = 0; i < array_size; i++) {
		JSON_Object* item = json_array_get_object(data_Array, i);
		if (item != NULL) {
			strcpy(data.noteBookName, json_object_get_string(item, "borrowNoteBook"));
			data.brokenNumber = (int)json_object_get_number(item, "brokenNum");
		}
		enqueue(queue, data);
	}
	json_value_free(root_Value);
}

void student_borrow_noteBook(QueueType* queue, int student_num, int current_day) {
	borrow data;
	noteBook noteBookData = dequeue(queue);
	if (strcmp(noteBookData.noteBookName, "") == 0 && noteBookData.brokenNumber == 0) {
		printf("=======================================\n");
		printf("�뿩 ������ ��Ʈ���� �����ϴ�.\n");
		printf("=======================================\n\n");
		return;
	}
	data.key = hash_function(student_num);
	strcpy(data.name, noteBookData.noteBookName);
	data.broken = noteBookData.brokenNumber;
	data.day = current_day;
	hash_chain_borrow_add(data, hash_table_borrow);
	printf("=======================================\n");
	printf("�뿩�� �й� : %d / �뿩�� ��Ʈ�� : %s / �ݳ� ������ 1���� �Դϴ�.\n", student_num, data.name);
	printf("=======================================\n\n");
}

int hash_chain_borrow_delete(int student_num, borrow* removed_item, int current_day) {
	// �ؽð� ���
	int hash_value = hash_function(student_num);

	// �ؽ� ���̺��� �ش� ��Ŷ ��������
	struct borrowList* node = hash_table_borrow[hash_value];
	struct borrowList* node_before = NULL;
	// ���� ����Ʈ Ž��
	while (node != NULL) {
		if (node->item.key == student_num) { // Ű�� ��ġ�ϸ�
			if (node_before == NULL) {
				hash_table_borrow[hash_value] = node->link;
			}
			else {
				node_before->link = node->link;
			}
			// ��� ��ȯ
			if (removed_item != NULL) {
				*removed_item = node->item; // �˻��� �����͸� result�� ����
				int borrow_day = node->item.day - current_day;
				if (abs(borrow_day) > -1 /*30*/) { // -1 �� �׽�Ʈ�� ���� ������Ʈ�� ���Ƿ� �߰��ϱ� ���� ����, ������ �뿩�Ⱓ�� 30
					printf("=======================================\n");
					printf("�ʰ��� �Ⱓ %d\n", borrow_day);
					printf("=======================================\n\n");
					json_file_rewriting(student_num);
				}
			}
			printf("=======================================\n");
			printf("�й� : %d / �ݳ� ��Ʈ�� : %s / �ݳ��Ϸ� �Ǿ����ϴ�.\n", student_num, node->item.name);
			printf("=======================================\n\n");
			free(node);
			return 1; // ����
		}
		node_before = node;
		node = node->link; // ���� ���� �̵�
	}

	// ��ġ�ϴ� �����Ͱ� ���� ���
	printf("=======================================\n");
	printf("�����Ϸ��� �׸��� �����ϴ�.\n");
	printf("=======================================\n\n");
	return 0; // ����
}


void student_return_noteBook(QueueType* queue, int student_num, int current_day) {
	borrow item;
	if (!hash_chain_borrow_delete(student_num, &item, current_day)) {
		printf("�й� �뿩 ���� ����\n\n");
		return;
	}
	noteBook noteBookData;
	strcpy(noteBookData.noteBookName, item.name);
	noteBookData.brokenNumber = item.broken;
	if (!is_full(queue)) {
		enqueue(queue, noteBookData);
	}
	else
		printf("ť ��ȭ\n\n");
}

int main() {
	int choice;
	int student_num;
	QueueType queue;
	init_queue(&queue);
	json_file_noteBook_reading(&queue);
	json_file_student_reading();

	time_t timer;
	struct tm* t;
	timer = time(NULL);
	t = localtime(&timer);
	int current = 0;
	while (1) {
		current = t->tm_yday;
		check_borrow_limit(current);
		printf("------������------\n1. �л���ȸ\n2. �뿩\n3. �뿩 ��Ȳ �˻�\n4. �ݳ�\n5. ���ο� �л� �߰�\n6. ����\n�Է� => ");
		scanf_s("%d", &choice);
		// �л� ��ȸ
		if (choice == 1) {
			printf("�й� �Է� => ");
			scanf_s("%d", &student_num);
			hash_chain_student_search(student_num);
		}
		// �뿩
		else if (choice == 2) {
			printf("�й� �Է� => ");
			scanf_s("%d", &student_num);
			student_borrow_noteBook(&queue, student_num, current);
		}
		// �뿩 ��Ȳ �˻�
		else if (choice == 3) {
			printf("�й� �Է� => ");
			scanf_s("%d", &student_num);
			hash_chain_borrow_search(student_num);
		}
		// �ݳ�
		else if (choice == 4) {
			printf("�й� �Է� => ");
			scanf_s("%d", &student_num);
			student_return_noteBook(&queue, student_num, current);
		}
		/*
		// �ݳ� �� �ڵ����� ������Ʈ
		// ��ü �л� ������Ʈ
		else if (choice == 5) {
			printf("�й� �Է� => ");
			scanf_s("%d", &student_num);
			json_file_rewriting(student_num);
			hash_chain_student_search(student_num);
		}
		*/
		// ���ο� �л� �߰�
		else if (choice == 5) {
			json_data_add();
		}
		// ����
		else if (choice == 6)
			break;
		else
			printf("�ٽ� �Է��Ͻʽÿ�\n\n");
	}
	return 0;
}