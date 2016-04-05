/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �ꥹ�����                                                        */
/*                                                                    */
/**********************************************************************/
#include "kern/kernel.h"

/** �ꥹ�Ⱥ��
 */
void
list_del(list_t *node) {

	node->next->prev = node->prev;
	node->prev->next = node->next;
	node->next = node->prev = node;
}

/** �ꥹ�Ȥ��������ɲ�
 */
void
list_add(struct _list_head *head, list_t *node) {

	node->next = (list_t *)head;
	node->prev = head->prev;
	head->prev->next = node;
	head->prev = node;
}

/** �ꥹ�Ȥ���Ƭ���ɲ�
 */
void
list_add_top(struct _list_head *head, list_t *node) {

	node->next = head->next;
	node->prev = head->prev;
	head->next->prev = node;
	head->next = node;
}

/** ��Ƭ�Ρ��ɻ���
 */
list_t *
list_ref_top(struct _list_head *head) {
	
	return head->next;
}

/** ��Ƭ�Ρ��ɳ���
 */
list_t *
list_get_top(struct _list_head *head) {
	list_t *top;

	top = list_ref_top(head);
	list_del(top);

	return top;
}

int
list_is_empty(struct _list_head *head) {
	
	return  (head->prev == (list_t *)head) && (head->next == (list_t *)head);
}

void 
list_rotate(struct _list_head *head) {

	list_add(head, list_get_top(head));
}

void 
init_list_node(list_t *node){

	node->prev = node->next = node;	
}

void 
init_list_head(list_head_t *head) {

	head->prev = head->next = (list_t *)head;	
}
