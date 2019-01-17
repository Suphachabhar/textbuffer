////////////////////////////////////////////////////////////////////////
// COMP2521 19T0 ... assignment 1: Textbuffer ADT
//
// yyyy-mm-dd	Your Name Here <z5555555@unsw.edu.au>

#include "textbuffer.h"
#include <assert.h>
#include <err.h>
#include <sysexits.h>
#include <stdio.h>
// your implementation goes here.

typedef struct text_node text_node;

typedef struct textbuffer
{
    size_t n_lines;
    size_t n_bytes;
    text_node *head, *tail;
} textbuffer;

typedef struct text_node
{
    char *text;
    size_t n_bytes;
    text_node *next;
    text_node *prev;
} text_node;

//create new text_node
static text_node *text_node_new(char *s);

//insert text_node at the end of textbuffer
static void textbuffer_insert_end(Textbuffer tb, char *s);

//print textbuffer
static void textbuffer_print(Textbuffer tb);

//print textbuffer from backward
static void textbuffer_print_backward(Textbuffer tb);

//replace matched substring
static char *string_replace(char *text, char *match, char *replace);

Textbuffer textbuffer_new(const char *text)
{
    textbuffer *tb = malloc(sizeof(*tb));
    if (tb == NULL)
        err(EX_OSERR, "couldn't allocate %zu bytes", sizeof *tb);
    (*tb) = (textbuffer){.n_lines = 0, .n_bytes = 0, .head = NULL, .tail = NULL};
    //remove last newline character
    if (strcmp(text, "") == 0)
        return tb;
    char *s, *token;
    s = strndup(text, strlen(text) - 1);
    //separate each line
    while ((token = strsep(&s, "\n")) != NULL)
    {
        textbuffer_insert_end(tb, token);
    }
    free(s);
    return tb;
}

static text_node *text_node_new(char *s)
{
    text_node *new = malloc(sizeof *new);
    if (new == NULL)
        err(EX_OSERR, "couldn't allocate text_node");
    (*new) = (text_node){.text = strdup(s), .n_bytes = strlen(s) + 1, .next = NULL, .prev = NULL};
    return new;
}

static void textbuffer_insert_end(Textbuffer tb, char *s)
{
    text_node *node = text_node_new(s);
    if (tb->head != NULL)
    {
        tb->tail->next = node;
        node->prev = tb->tail;
    }
    else
    {
        tb->head = node;
    }
    tb->tail = node;
    tb->n_lines++;
    tb->n_bytes += node->n_bytes;
}

static void textbuffer_print(Textbuffer tb)
{
    assert(tb != NULL);
    text_node *curr = tb->head;
    if (tb->head && tb->tail)
        printf("head = [%s] tail = [%s]\n", tb->head->text, tb->tail->text);
    printf("n_lines = [%zu]\n", tb->n_lines);
    printf("n_bytes = [%zu]\n", tb->n_bytes);
    while (curr != NULL)
    {
        printf("[%s]->", curr->text);
        curr = curr->next;
    }
    puts("|");
    textbuffer_print_backward(tb);
}

static void textbuffer_print_backward(Textbuffer tb)
{
    assert(tb != NULL);
    text_node *curr = tb->tail;
    while (curr != NULL)
    {
        printf("[%s]->", curr->text);
        curr = curr->prev;
    }
    puts("|");
}

void textbuffer_drop(Textbuffer tb)
{
    assert(tb != NULL);
    text_node *curr = tb->head;
    while (curr != NULL)
    {
        text_node *tmp = curr->next;
        free(curr);
        curr = tmp;
    }
    free(tb);
}

size_t textbuffer_lines(Textbuffer tb)
{
    assert(tb != NULL);
    return tb->n_lines;
}

size_t textbuffer_bytes(Textbuffer tb)
{
    return tb->n_bytes;
}

char *textbuffer_to_str(Textbuffer tb)
{
    assert(tb != NULL);
    char *str = strdup("");
    text_node *curr = tb->head;
    size_t i = 0;
    while (curr != NULL)
    {
        // printf("%d\n",i);
        i++;
        str = realloc(str, sizeof(char) * (strlen(str) + strlen(curr->text) + 2));
        strcat(str, curr->text);
        strcat(str, "\n");
        curr = curr->next;
    }
    return str;
}

void textbuffer_swap(Textbuffer tb, size_t pos1, size_t pos2)
{
    assert(tb != NULL);
    if (pos1 < 0 || pos1 >= tb->n_lines || pos2 < 0 || pos2 >= tb->n_lines)
    {
        fprintf(stderr, "index out of range\n");
        abort();
    }
    if (pos1 == pos2)
        return;
    text_node *curr = tb->head;
    text_node *p1 = NULL, *p2 = NULL, *tmp;
    for (size_t i = 0; i < tb->n_lines; i++)
    {
        if (i == pos1)
            p1 = curr;
        if (i == pos2)
            p2 = curr;
        curr = curr->next;
    }

    tmp = p1->next;
    p1->next = p2->next;
    p2->next = tmp;
    if (p1->next != NULL)
        p1->next->prev = p1;
    else
        tb->tail = p1;
    if (p2->next != NULL)
        p2->next->prev = p2;
    else
        tb->tail = p2;

    tmp = p1->prev;
    p1->prev = p2->prev;
    p2->prev = tmp;
    if (p1->prev != NULL)
        p1->prev->next = p1;
    else
        tb->head = p1;
    if (p2->prev != NULL)
        p2->prev->next = p2;
    else
        tb->head = p2;
}

void textbuffer_insert(Textbuffer tb1, size_t pos, Textbuffer tb2)
{
    assert(tb1 != NULL);
    assert(tb2 != NULL);
    assert(tb1 != tb2);
    if (pos < 0 || pos > tb1->n_lines)
    {
        fprintf(stderr, "pos is out of range\n");
        abort();
    }
    if (pos == tb1->n_lines)
    {
        tb1->tail->next = tb2->head;
        tb2->head->prev = tb1->tail;
        tb1->tail = tb2->tail;
    }
    else
    {
        text_node *curr = tb1->head;
        for (size_t i = 0; i < pos; i++)
        {
            curr = curr->next;
        }
        if (curr->prev == NULL)
            tb1->head = tb2->head;
        else
        {
            curr->prev->next = tb2->head;
            tb2->head->prev = curr->prev;
        }
        tb2->tail->next = curr;
        curr->prev = tb2->tail;
    }
    tb1->n_bytes += tb2->n_bytes;
    tb1->n_lines += tb2->n_lines;

    tb2->head = NULL;
    tb2->tail = NULL;
    free(tb2);
    return;
}

void textbuffer_paste(Textbuffer tb1, size_t pos, Textbuffer tb2)
{
    assert(tb1 != NULL);
    assert(tb2 != NULL);
    assert(tb1 != tb2);
    if (pos < 0 || pos > tb1->n_lines)
    {
        fprintf(stderr, "pos is out of range\n");
        abort();
    }
    text_node *insert_node = tb1->head, *prev_insert = NULL;
    for (size_t i = 0; i < pos; i++)
    {
        prev_insert = insert_node;
        insert_node = insert_node->next;
    }
    text_node *new_curr, *tb2_curr = tb2->head;
    if (prev_insert == NULL)
    {
        //insert to head
        tb1->head = text_node_new(tb2_curr->text);
        new_curr = tb1->head;
    }
    else
    {
        text_node *new = text_node_new(tb2_curr->text);
        prev_insert->next = new;
        new->prev = prev_insert;
        new_curr = new;
    }
    for (size_t i = 1; i < tb2->n_lines; i++)
    {
        tb2_curr = tb2_curr->next;
        text_node *new = text_node_new(tb2_curr->text);
        new_curr->next = new;
        new->prev = new_curr;
        new_curr = new_curr->next;
    }

    if (insert_node == NULL)
        tb1->tail = new_curr;
    else
    {
        new_curr->next = insert_node;
        insert_node->prev = new_curr;
    }

    tb1->n_bytes += tb2->n_bytes;
    tb1->n_lines += tb2->n_lines;

    return;
}

Textbuffer textbuffer_cut(Textbuffer tb, size_t from, size_t to)
{
    assert(tb != NULL);
    if (from < 0 || from >= tb->n_lines)
    {
        fprintf(stderr, "from is out of range\n");
        abort();
    }
    if (to < 0 || to >= tb->n_lines)
    {
        fprintf(stderr, "to is out of range\n");
        abort();
    }
    Textbuffer tb_cut = textbuffer_new("");
    if (from > to)
        return tb_cut;
    text_node *prev_from = NULL, *after_to = NULL, *from_node = NULL, *to_node = NULL;
    text_node *curr = tb->head;
    size_t cut_bytes = 0;
    size_t cut_lines = to - from + 1;
    for (size_t i = 0; i < tb->n_lines; i++)
    {
        if (i == from - 1)
            prev_from = curr;
        if (i == from)
            from_node = curr;
        if (i == to + 1)
            after_to = curr;
        if (i == to)
            to_node = curr;
        if (i >= from && i <= to)
            cut_bytes += curr->n_bytes;
        curr = curr->next;
    }
    // printf("prev from = %s\n", prev_from->text);
    // printf("from = %s\n", from_node->text);
    // printf("to = %s\n", to_node->text);
    // printf("after to = %s\n", after_to->text);

    to_node->next = NULL;
    from_node->prev = NULL;
    tb_cut->head = from_node;
    tb_cut->tail = to_node;
    tb_cut->n_lines = cut_lines;
    tb_cut->n_bytes = cut_bytes;

    tb->n_lines -= cut_lines;
    tb->n_bytes -= cut_bytes;

    if (prev_from == NULL)
        tb->head = after_to;
    else
        prev_from->next = after_to;
    if (after_to == NULL)
        tb->tail = prev_from;
    else
        after_to->prev = prev_from;

    return tb_cut;
}

Textbuffer textbuffer_copy(Textbuffer tb, size_t from, size_t to)
{
    assert(tb != NULL);
    if (from < 0 || from >= tb->n_lines)
    {
        fprintf(stderr, "from is out of range\n");
        abort();
    }
    if (to < 0 || to >= tb->n_lines)
    {
        fprintf(stderr, "to is out of range\n");
        abort();
    }
    Textbuffer tb_copy = textbuffer_new("");
    if (from > to)
        return tb_copy;
    text_node *from_node = NULL, *to_node = NULL;
    text_node *curr = tb->head;
    for (size_t i = 0; i < tb->n_lines; i++)
    {
        if (i == from)
            from_node = curr;
        if (i == to)
            to_node = curr;
        curr = curr->next;
    }
    for (text_node *copy_node = from_node; copy_node != to_node; copy_node = copy_node->next)
    {
        textbuffer_insert_end(tb_copy, copy_node->text);
    }
    textbuffer_insert_end(tb_copy, to_node->text);
    return tb_copy;
}

void textbuffer_delete(Textbuffer tb, size_t from, size_t to)
{
    assert(tb != NULL);
    if (from < 0 || from >= tb->n_lines)
    {
        fprintf(stderr, "from is out of range\n");
        abort();
    }
    if (to < 0 || to >= tb->n_lines)
    {
        fprintf(stderr, "to is out of range\n");
        abort();
    }
    if (from > to)
        return;
    text_node *prev_from = NULL, *after_to = NULL;
    text_node *curr = tb->head;
    size_t cut_bytes = 0;
    size_t cut_lines = to - from + 1;
    for (size_t i = 0; i < tb->n_lines; i++)
    {
        text_node *tmp = curr->next;
        if (i == from - 1)
            prev_from = curr;
        if (i == to + 1)
            after_to = curr;

        if (i >= from && i <= to)
        {
            cut_bytes += curr->n_bytes;
            free(curr);
        }
        curr = tmp;
    }
    // printf("prev from = %s\n", prev_from->text);
    // printf("after to = %s\n", after_to->text);

    tb->n_lines -= cut_lines;
    tb->n_bytes -= cut_bytes;

    if (prev_from == NULL)
        tb->head = after_to;
    else
        prev_from->next = after_to;
    if (after_to == NULL)
        tb->tail = prev_from;
    else
        after_to->prev = prev_from;
}

ssize_t textbuffer_search(Textbuffer tb, char *match, bool rev)
{
    ssize_t n_found = 0;
    text_node *curr;
    if (rev)
        curr = tb->tail;
    else
        curr = tb->head;
    while (curr != NULL)
    {
        char *f = strstr(curr->text, match);
        if (f)
            n_found++;
        if (rev)
            curr = curr->prev;
        else
            curr = curr->next;
    }
    return n_found ? n_found : -1;
}

static char *string_replace(char *text, char *match, char *replace)
{
    char *res = strdup(text); /* return value is always allocated */
    char *p, *q;
    size_t offset = 0;
    size_t len_text = strlen(text);
    size_t len_match = strlen(match);
    size_t len_replace = strlen(replace);
    if (len_match == 0)
        return res;
    while ((p = strstr(res + offset, match)) != NULL)
    {
        offset = (size_t)(p - res);
        if (len_match == len_replace)
        {
            /* no need to reallocate, replace in place */
            memcpy(res + offset, replace, len_replace);
        }
        else
        {
            // printf("%s\n",p);
            /* allocate a new array with the adjusted length */
            q = malloc(sizeof(char) * (len_text + len_replace - len_match + 1));
            /* copy the beginning of the string */
            memcpy(q, res, offset);
            /* copy the replacement string */
            memcpy(q + offset, replace, len_replace);
            /* copy the remainder of the string, and the final '\0' */
            memcpy(q + offset + len_replace, res + offset + len_match, len_text - offset - len_match + 1);
            /* free the previous string */
            free(res);
            res = q;
            len_text = strlen(res);
        }
        /* search for matches from the end of the replacement */
        // printf("res : %s\n",res);
        offset += len_replace;
    }
    free(text);
    return res;
}

void textbuffer_replace(Textbuffer tb, char *match, char *replace)
{
    text_node *curr = tb->head;
    for (size_t i = 0; i < tb->n_lines; i++)
    {
        curr->text = strdup(string_replace(curr->text, match, replace));
        size_t tmp = curr->n_bytes;
        curr->n_bytes = strlen(curr->text);
        tb->n_bytes += curr->n_bytes - tmp + 1;
        curr = curr->next;
    }
}

void white_box_tests(void)
{
    puts("=== Whitebox Tests start ===");
    puts("Test 0 : general textbuffer");
    Textbuffer tb = textbuffer_new("abcd\nefgh\n");
    assert(textbuffer_bytes(tb) == tb->n_bytes);
    assert(textbuffer_lines(tb) == tb->n_lines);
    assert(tb->n_lines == 2);
    assert(tb->n_bytes == 10);
    assert(strcmp(tb->head->text, "abcd") == 0);
    assert(strcmp(tb->tail->text, "efgh") == 0);
    assert(tb->head->next == tb->tail);
    assert(tb->tail->prev == tb->head);
    int i = 0;
    char *texts[] = {
        "abcd",
        "efgh"};
    for (text_node *curr = tb->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts[i]) == 0);
        assert(strlen(curr->text) == strlen(texts[i]));
    }
    textbuffer_print(tb);
    textbuffer_drop(tb);

    puts("Test 1 : textbuffer_swap");
    Textbuffer tb1_1 = textbuffer_new("1\n2\n3\n4\n5\n");
    textbuffer_swap(tb1_1, 0, 2);
    assert(tb1_1->n_lines == 5);
    assert(tb1_1->n_bytes == 10);
    i = 0;
    char *texts1[] = {"3", "2", "1", "4", "5"};
    for (text_node *curr = tb1_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts1[i]) == 0);
        assert(strlen(curr->text) == strlen(texts1[i]));
    }
    assert(strcmp(tb1_1->head->text, "3") == 0);
    assert(strcmp(tb1_1->tail->text, "5") == 0);
    assert(tb1_1->n_lines == 5);
    assert(tb1_1->n_bytes == 10);

    textbuffer_swap(tb1_1, 0, 4);
    i = 0;
    char *texts2[] = {"5", "2", "1", "4", "3"};
    for (text_node *curr = tb1_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts2[i]) == 0);
        assert(strlen(curr->text) == strlen(texts2[i]));
    }
    assert(strcmp(tb1_1->head->text, "5") == 0);
    assert(strcmp(tb1_1->tail->text, "3") == 0);
    assert(tb1_1->n_lines == 5);
    assert(tb1_1->n_bytes == 10);

    textbuffer_swap(tb1_1, 3, 2);
    i = 0;
    char *texts3[] = {"5", "2", "4", "1", "3"};
    for (text_node *curr = tb1_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts3[i]) == 0);
        assert(strlen(curr->text) == strlen(texts3[i]));
    }
    assert(strcmp(tb1_1->head->text, "5") == 0);
    assert(strcmp(tb1_1->tail->text, "3") == 0);
    assert(tb1_1->n_lines == 5);
    assert(tb1_1->n_bytes == 10);

    textbuffer_swap(tb1_1, 4, 1);
    i = 0;
    char *texts4[] = {"5", "3", "4", "1", "2"};
    for (text_node *curr = tb1_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts4[i]) == 0);
        assert(strlen(curr->text) == strlen(texts4[i]));
    }
    assert(strcmp(tb1_1->head->text, "5") == 0);
    assert(strcmp(tb1_1->tail->text, "2") == 0);
    assert(tb1_1->n_lines == 5);
    assert(tb1_1->n_bytes == 10);
    // swap out of range
    // textbuffer_swap(tb1_1, 0, tb1_1->n_lines + 3);
    textbuffer_drop(tb1_1);

    puts("Test 2 : textbuffer_insert");
    //insert at the end
    Textbuffer tb2_1 = textbuffer_new("1\n2\n3\n4\n5\n");
    Textbuffer tb2_2 = textbuffer_new("6\n7\n8\n9\n10\n");
    textbuffer_insert(tb2_1, tb2_1->n_lines, tb2_2);
    assert(strcmp(tb2_1->head->text, "1") == 0);
    assert(strcmp(tb2_1->tail->text, "10") == 0);
    assert(tb2_1->n_lines == 10);
    assert(tb2_1->n_bytes == 21);
    i = 0;
    char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb2_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    textbuffer_drop(tb2_1);

    //insert at the begin
    Textbuffer tb2_3 = textbuffer_new("1\n2\n3\n4\n5\n");
    Textbuffer tb2_4 = textbuffer_new("6\n7\n8\n9\n10\n");
    textbuffer_insert(tb2_3, 0, tb2_4);
    i = 0;
    char *texts6[] = {"6", "7", "8", "9", "10", "1", "2", "3", "4", "5"};
    for (text_node *curr = tb2_3->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts6[i]) == 0);
        assert(strlen(curr->text) == strlen(texts6[i]));
    }
    assert(strcmp(tb2_3->head->text, "6") == 0);
    assert(strcmp(tb2_3->tail->text, "5") == 0);
    assert(tb2_3->n_lines == 10);
    assert(tb2_3->n_bytes == 21);
    textbuffer_drop(tb2_3);

    //insert between
    Textbuffer tb2_5 = textbuffer_new("1\n2\n3\n4\n5\n");
    Textbuffer tb2_6 = textbuffer_new("6\n7\n8\n9\n10\n");
    textbuffer_insert(tb2_5, 2, tb2_6);
    i = 0;
    char *texts7[] = {"1", "2", "6", "7", "8", "9", "10", "3", "4", "5"};
    for (text_node *curr = tb2_5->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts7[i]) == 0);
        assert(strlen(curr->text) == strlen(texts7[i]));
    }
    assert(strcmp(tb2_5->head->text, "1") == 0);
    assert(strcmp(tb2_5->tail->text, "5") == 0);
    assert(tb2_5->n_lines == 10);
    assert(tb2_5->n_bytes == 21);
    textbuffer_drop(tb2_5);

    //insert out of range
    // Textbuffer tb2_7 = textbuffer_new("1\n2\n3\n4\n5\n");
    // Textbuffer tb2_8 = textbuffer_new("6\n7\n8\n9\n10\n");
    // textbuffer_insert(tb2_7, 10, tb2_8);

    puts("Test 3 : textbuffer_paste");
    //paste at the end
    Textbuffer tb3_1 = textbuffer_new("1\n2\n3\n");
    Textbuffer tb3_2 = textbuffer_new("4\n5\n\n");
    textbuffer_paste(tb3_1, tb3_1->n_lines, tb3_2);
    i = 0;
    char *texts8[] = {"1", "2", "3", "4", "5", ""};
    for (text_node *curr = tb3_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts8[i]) == 0);
        assert(strlen(curr->text) == strlen(texts8[i]));
    }
    i = 0;
    char *texts9[] = {"4", "5", ""};
    for (text_node *curr = tb3_2->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts9[i]) == 0);
        assert(strlen(curr->text) == strlen(texts9[i]));
    }
    assert(strcmp(tb3_1->head->text, "1") == 0);
    assert(strcmp(tb3_1->tail->text, "") == 0);
    assert(tb3_1->n_lines == 6);
    assert(tb3_1->n_bytes == 11);
    assert(strcmp(tb3_2->head->text, "4") == 0);
    assert(strcmp(tb3_2->tail->text, "") == 0);
    assert(tb3_2->n_lines == 3);
    assert(tb3_2->n_bytes == 5);
    textbuffer_drop(tb3_1);
    textbuffer_drop(tb3_2);

    //paste at the begin
    tb3_1 = textbuffer_new("1\n2\n3\n");
    tb3_2 = textbuffer_new("4\n5\n\n");
    textbuffer_paste(tb3_1, 0, tb3_2);
    i = 0;
    char *texts10[] = {"4", "5", "", "1", "2", "3"};
    for (text_node *curr = tb3_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts10[i]) == 0);
        assert(strlen(curr->text) == strlen(texts10[i]));
    }
    i = 0;
    //char *texts9[] = {"4", "5", ""};
    for (text_node *curr = tb3_2->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts9[i]) == 0);
        assert(strlen(curr->text) == strlen(texts9[i]));
    }
    assert(strcmp(tb3_1->head->text, "4") == 0);
    assert(strcmp(tb3_1->tail->text, "3") == 0);
    assert(tb3_1->n_lines == 6);
    assert(tb3_1->n_bytes == 11);
    assert(strcmp(tb3_2->head->text, "4") == 0);
    assert(strcmp(tb3_2->tail->text, "") == 0);
    assert(tb3_2->n_lines == 3);
    assert(tb3_2->n_bytes == 5);
    textbuffer_drop(tb3_1);
    textbuffer_drop(tb3_2);

    //paste between
    tb3_1 = textbuffer_new("1\n2\n3\n");
    tb3_2 = textbuffer_new("4\n5\n\n");
    textbuffer_paste(tb3_1, 1, tb3_2);
    i = 0;
    char *texts11[] = {"1", "4", "5", "", "2", "3"};
    for (text_node *curr = tb3_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts11[i]) == 0);
        assert(strlen(curr->text) == strlen(texts11[i]));
    }
    i = 0;
    //char *texts9[] = {"4", "5", ""};
    for (text_node *curr = tb3_2->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts9[i]) == 0);
        assert(strlen(curr->text) == strlen(texts9[i]));
    }
    assert(strcmp(tb3_1->head->text, "1") == 0);
    assert(strcmp(tb3_1->tail->text, "3") == 0);
    assert(tb3_1->n_lines == 6);
    assert(tb3_1->n_bytes == 11);
    assert(strcmp(tb3_2->head->text, "4") == 0);
    assert(strcmp(tb3_2->tail->text, "") == 0);
    assert(tb3_2->n_lines == 3);
    assert(tb3_2->n_bytes == 5);
    textbuffer_drop(tb3_1);
    textbuffer_drop(tb3_2);

    //paste out of range
    // tb3_1 = textbuffer_new("1\n2\n3\n");
    // tb3_2 = textbuffer_new("4\n5\n\n");
    // textbuffer_paste(tb3_1, 100, tb3_2);

    puts("Test 4 : textbuffer_cut");
    //cut all
    Textbuffer tb4_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    Textbuffer tb4_2 = textbuffer_cut(tb4_1, 0, tb4_1->n_lines - 1);
    i = 0;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb4_2->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    assert(tb4_1->head == NULL);
    assert(tb4_1->tail == NULL);
    assert(tb4_1->n_lines == 0);
    assert(tb4_1->n_bytes == 0);
    assert(strcmp(tb4_2->head->text, "1") == 0);
    assert(strcmp(tb4_2->tail->text, "6") == 0);
    assert(tb4_2->n_lines == 6);
    assert(tb4_2->n_bytes == 12);
    textbuffer_drop(tb4_1);
    textbuffer_drop(tb4_2);

    //cut between
    tb4_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    tb4_2 = textbuffer_cut(tb4_1, 1, 3);
    i = 0;
    char *texts12[] = {"1", "5", "6"};
    for (text_node *curr = tb4_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts12[i]) == 0);
        assert(strlen(curr->text) == strlen(texts12[i]));
    }
    i = 1;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb4_2->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    assert(strcmp(tb4_1->head->text, "1") == 0);
    assert(strcmp(tb4_1->tail->text, "6") == 0);
    assert(tb4_1->n_lines == 3);
    assert(tb4_1->n_bytes == 6);
    assert(strcmp(tb4_2->head->text, "2") == 0);
    assert(strcmp(tb4_2->tail->text, "4") == 0);
    assert(tb4_2->n_lines == 3);
    assert(tb4_2->n_bytes == 6);
    textbuffer_drop(tb4_1);
    textbuffer_drop(tb4_2);

    //from > to
    tb4_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    tb4_2 = textbuffer_cut(tb4_1, 3, 1);
    i = 0;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb4_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    assert(strcmp(tb4_1->head->text, "1") == 0);
    assert(strcmp(tb4_1->tail->text, "6") == 0);
    assert(tb4_1->n_lines == 6);
    assert(tb4_1->n_bytes == 12);
    assert(tb4_2->head == NULL);
    assert(tb4_2->tail == NULL);
    assert(tb4_2->n_lines == 0);
    assert(tb4_2->n_bytes == 0);
    textbuffer_drop(tb4_1);
    textbuffer_drop(tb4_2);

    //from outbound
    // tb4_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // tb4_2 = textbuffer_cut(tb4_1, -1, 1);

    //to outbound
    // tb4_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // tb4_2 = textbuffer_cut(tb4_1, 0, 6);

    puts("Test 5 : textbuffer_copy");
    //copy all
    Textbuffer tb5_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    Textbuffer tb5_2 = textbuffer_copy(tb5_1, 0, tb5_1->n_lines - 1);
    i = 0;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb5_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    i = 0;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb5_2->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    assert(tb5_1 != tb5_2);
    assert(strcmp(tb5_1->head->text, "1") == 0);
    assert(strcmp(tb5_1->tail->text, "6") == 0);
    assert(tb5_1->n_lines == 6);
    assert(tb5_1->n_bytes == 12);
    assert(strcmp(tb5_2->head->text, "1") == 0);
    assert(strcmp(tb5_2->tail->text, "6") == 0);
    assert(tb5_2->n_lines == 6);
    assert(tb5_2->n_bytes == 12);
    textbuffer_drop(tb5_1);
    textbuffer_drop(tb5_2);

    //copy between
    tb5_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    tb5_2 = textbuffer_copy(tb5_1, 2, 3);
    i = 0;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb5_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    i = 2;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb5_2->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    assert(strcmp(tb5_1->head->text, "1") == 0);
    assert(strcmp(tb5_1->tail->text, "6") == 0);
    assert(tb5_1->n_lines == 6);
    assert(tb5_1->n_bytes == 12);
    assert(strcmp(tb5_2->head->text, "3") == 0);
    assert(strcmp(tb5_2->tail->text, "4") == 0);
    assert(tb5_2->n_lines == 2);
    assert(tb5_2->n_bytes == 4);
    textbuffer_drop(tb5_1);
    textbuffer_drop(tb5_2);

    //from > to
    tb5_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    tb5_2 = textbuffer_copy(tb5_1, 4, 3);
    i = 0;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb5_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    assert(strcmp(tb5_1->head->text, "1") == 0);
    assert(strcmp(tb5_1->tail->text, "6") == 0);
    assert(tb5_1->n_lines == 6);
    assert(tb5_1->n_bytes == 12);
    assert(tb5_2->head == NULL);
    assert(tb5_2->tail == NULL);
    assert(tb5_2->n_lines == 0);
    assert(tb5_2->n_bytes == 0);
    textbuffer_drop(tb5_1);
    textbuffer_drop(tb5_2);

    //from outbound
    // tb5_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // tb5_2 = textbuffer_copy(tb5_1, -1, 1);

    //to outbound
    // tb5_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // tb5_2 = textbuffer_copy(tb5_1, 2, 7);

    puts("Test 6 : textbuffer_delete");
    //delete all
    Textbuffer tb6_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    textbuffer_delete(tb6_1, 0, tb6_1->n_lines - 1);
    assert(tb6_1->head == NULL);
    assert(tb6_1->tail == NULL);
    assert(tb6_1->n_lines == 0);
    assert(tb6_1->n_bytes == 0);
    textbuffer_drop(tb6_1);

    //delete between
    tb6_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    textbuffer_delete(tb6_1, 1, 3);
    i = 0;
    // char *texts12[] = {"1", "5", "6"};
    for (text_node *curr = tb6_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts12[i]) == 0);
        assert(strlen(curr->text) == strlen(texts12[i]));
    }
    assert(strcmp(tb6_1->head->text, "1") == 0);
    assert(strcmp(tb6_1->tail->text, "6") == 0);
    assert(tb6_1->n_lines == 3);
    assert(tb6_1->n_bytes == 6);
    textbuffer_drop(tb6_1);

    //from > to
    tb6_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    textbuffer_delete(tb6_1, 3, 1);
    i = 0;
    //char *texts5[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    for (text_node *curr = tb6_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts5[i]) == 0);
        assert(strlen(curr->text) == strlen(texts5[i]));
    }
    assert(strcmp(tb6_1->head->text, "1") == 0);
    assert(strcmp(tb6_1->tail->text, "6") == 0);
    assert(tb6_1->n_lines == 6);
    assert(tb6_1->n_bytes == 12);
    textbuffer_drop(tb6_1);

    //from outbound
    // tb6_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // textbuffer_delete(tb6_1, -1, 1);

    //to outbound
    // tb6_1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // textbuffer_delete(tb6_1, 0, 8);

    puts("Test 7 : textbuffer_search");
    //found some line
    Textbuffer tb7_1 = textbuffer_new("Hello guy\nGood Guy\nBad guy\ngood\ngguyy\n\n");
    assert(textbuffer_search(tb7_1, "guy", 0) == 3);
    assert(textbuffer_search(tb7_1, "guy", 1) == 3);
    textbuffer_drop(tb7_1);

    //Search by empty string (all lines match)
    tb7_1 = textbuffer_new("Hello guy\nGood Guy\nBad guy\ngood\ngguyy\n\n");
    assert(textbuffer_search(tb7_1, "", 0) == (int)(tb7_1->n_lines));
    assert(textbuffer_search(tb7_1, "", 1) == (int)(tb7_1->n_lines));
    textbuffer_drop(tb7_1);

    //Search not found
    tb7_1 = textbuffer_new("Hello guy\nGood Guy\nBad guy\ngood\ngguyy\n\n");
    assert(textbuffer_search(tb7_1, "gag", 0) == -1);
    assert(textbuffer_search(tb7_1, "gag", 1) == -1);
    textbuffer_drop(tb7_1);

    puts("Test 8 : textbuffer_replace");
    //replace with same length text
    Textbuffer tb8_1 = textbuffer_new("Hello world world!!!\nHello world\nGood World\nGood Bye\n");
    textbuffer_replace(tb8_1, "world", "earth");
    i = 0;
    char *texts13[] = {"Hello earth earth!!!", "Hello earth", "Good World", "Good Bye"};
    for (text_node *curr = tb8_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts13[i]) == 0);
        assert(strlen(curr->text) == strlen(texts13[i]));
    }
    assert(strcmp(tb8_1->head->text, "Hello earth earth!!!") == 0);
    assert(strcmp(tb8_1->tail->text, "Good Bye") == 0);
    assert(tb8_1->n_lines == 4);
    assert(tb8_1->n_bytes == 53);
    textbuffer_drop(tb8_1);

    // //replace with smaller text
    tb8_1 = textbuffer_new("Hello world world!!!\nHello world\nGood World\nGood Bye\n");
    textbuffer_replace(tb8_1, "world", "unsw");
    assert(strcmp(textbuffer_to_str(tb8_1), "Hello unsw unsw!!!\nHello unsw\nGood World\nGood Bye\n") == 0);
    i = 0;
    char *texts14[] = {"Hello unsw unsw!!!", "Hello unsw", "Good World", "Good Bye"};
    for (text_node *curr = tb8_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts14[i]) == 0);
        assert(strlen(curr->text) == strlen(texts14[i]));
    }
    assert(strcmp(tb8_1->head->text, "Hello unsw unsw!!!") == 0);
    assert(strcmp(tb8_1->tail->text, "Good Bye") == 0);
    assert(tb8_1->n_lines == 4);
    assert(tb8_1->n_bytes == 50);
    textbuffer_drop(tb8_1);

    //replace with bigger text
    tb8_1 = textbuffer_new("Hello world world!!!\nHello world\nGood World\nGood Bye\n");
    textbuffer_replace(tb8_1, "world", "SydneySydneySydney");
    i = 0;
    char *texts15[] = {"Hello SydneySydneySydney SydneySydneySydney!!!", "Hello SydneySydneySydney", "Good World", "Good Bye"};
    for (text_node *curr = tb8_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts15[i]) == 0);
        assert(strlen(curr->text) == strlen(texts15[i]));
    }
    assert(strcmp(tb8_1->head->text, "Hello SydneySydneySydney SydneySydneySydney!!!") == 0);
    assert(strcmp(tb8_1->tail->text, "Good Bye") == 0);
    assert(tb8_1->n_lines == 4);
    assert(tb8_1->n_bytes == 92);
    textbuffer_drop(tb8_1);

    // //replace with empty text
    tb8_1 = textbuffer_new("Hello world world!!!\nHello world\nGood World\nGood Bye\n");
    textbuffer_replace(tb8_1, "world", "");
    i = 0;
    char *texts16[] = {"Hello  !!!", "Hello ", "Good World", "Good Bye"};
    for (text_node *curr = tb8_1->head; curr != NULL; curr = curr->next, i++)
    {
        assert(strcmp(curr->text, texts16[i]) == 0);
        assert(strlen(curr->text) == strlen(texts16[i]));
    }
    assert(strcmp(tb8_1->head->text, "Hello  !!!") == 0);
    assert(strcmp(tb8_1->tail->text, "Good Bye") == 0);
    assert(tb8_1->n_lines == 4);
    assert(tb8_1->n_bytes == 38);
    textbuffer_drop(tb8_1);
    puts("=== All Whitebox Tests passed!! ===");
}