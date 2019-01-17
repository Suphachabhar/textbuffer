////////////////////////////////////////////////////////////////////////
// COMP2521 19T0 ... assignment 1: Textbuffer ADT
//
// yyyy-mm-dd	Your Name Here <z5555555@unsw.edu.au>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "textbuffer.h"

static void test_text_buffer_general(void);
static void test_text_buffer_swap(void);
static void test_text_buffer_insert(void);
static void test_text_buffer_paste(void);
static void test_text_buffer_cut(void);
static void test_text_buffer_copy(void);
static void test_text_buffer_delete(void);
static void test_text_buffer_search(void);
static void test_text_buffer_replace(void);

int main(void)
{
	// your tests go here!
	white_box_tests();

	test_text_buffer_general();
	test_text_buffer_swap();
	test_text_buffer_insert();
	test_text_buffer_paste();
	test_text_buffer_cut();
	test_text_buffer_copy();
	test_text_buffer_delete();
	test_text_buffer_search();
	test_text_buffer_replace();

	puts("\nAll tests passed. You are awesome!");
}

static void test_text_buffer_general(void)
{
	Textbuffer tb = textbuffer_new("Hello\nWorld\n");
	assert(textbuffer_bytes(tb) == 12);
	assert(textbuffer_lines(tb) == 2);
	assert(strcmp(textbuffer_to_str(tb), "Hello\nWorld\n") == 0);
	textbuffer_drop(tb);
}
static void test_text_buffer_swap(void)
{
	Textbuffer tb = textbuffer_new("a\nb\nc\nd\ne\n");
	textbuffer_swap(tb, 0, 1);
	assert(textbuffer_lines(tb) == 5);
	assert(textbuffer_bytes(tb) == 10);
	assert(strcmp(textbuffer_to_str(tb), "b\na\nc\nd\ne\n") == 0);
	assert(textbuffer_lines(tb) == 5);
	assert(textbuffer_bytes(tb) == 10);

	textbuffer_swap(tb, 0, 4);
	assert(strcmp(textbuffer_to_str(tb), "e\na\nc\nd\nb\n") == 0);
	assert(textbuffer_lines(tb) == 5);
	assert(textbuffer_bytes(tb) == 10);

	textbuffer_swap(tb, 3, 2);
	assert(strcmp(textbuffer_to_str(tb), "e\na\nd\nc\nb\n") == 0);
	assert(textbuffer_lines(tb) == 5);
	assert(textbuffer_bytes(tb) == 10);

	textbuffer_swap(tb, 4, 1);
	assert(strcmp(textbuffer_to_str(tb), "e\nb\nd\nc\na\n") == 0);
	assert(textbuffer_lines(tb) == 5);
	assert(textbuffer_bytes(tb) == 10);
	// swap out of range
	// textbuffer_swap(tb, 0, textbuffer_lines(tb) + 3);
	textbuffer_drop(tb);
}
static void test_text_buffer_insert(void)
{
	//insert at the end
	Textbuffer tb1 = textbuffer_new("1\n2\n3\n4\n5\n");
	Textbuffer tb2 = textbuffer_new("6\n7\n8\n9\n10\n");
	textbuffer_insert(tb1, textbuffer_lines(tb2), tb2);
	assert(textbuffer_lines(tb1) == 10);
	assert(textbuffer_bytes(tb1) == 21);
	assert(strcmp(textbuffer_to_str(tb1), "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n") == 0);
	textbuffer_drop(tb1);

	//insert at the begin
	Textbuffer tb3 = textbuffer_new("abc\n");
	Textbuffer tb4 = textbuffer_new("def\n");
	textbuffer_insert(tb3, 0, tb4);
	assert(strcmp(textbuffer_to_str(tb3), "def\nabc\n") == 0);
	assert(textbuffer_lines(tb3) == 2);
	assert(textbuffer_bytes(tb3) == 8);
	textbuffer_drop(tb3);

	// //insert between
	Textbuffer tb5 = textbuffer_new("abc\nhij\n");
	Textbuffer tb6 = textbuffer_new("def\n");
	textbuffer_insert(tb5, 1, tb6);
	assert(strcmp(textbuffer_to_str(tb5), "abc\ndef\nhij\n") == 0);
	assert(textbuffer_lines(tb5) == 3);
	assert(textbuffer_bytes(tb5) == 12);
	textbuffer_drop(tb5);

	//insert out of range
	// Textbuffer tb7 = textbuffer_new("1\n2\n3\n4\n5\n");
	// Textbuffer tb8 = textbuffer_new("6\n7\n8\n9\n10\n");
	// textbuffer_insert(tb7, 10, tb8);
}
static void test_text_buffer_paste(void)
{
	//paste at the end
	Textbuffer tb1 = textbuffer_new("1\n2\n3\n");
	Textbuffer tb2 = textbuffer_new("4\n5\n\n");
	textbuffer_paste(tb1, textbuffer_lines(tb1), tb2);
	assert(strcmp(textbuffer_to_str(tb1), "1\n2\n3\n4\n5\n\n") == 0);
	assert(strcmp(textbuffer_to_str(tb2), "4\n5\n\n") == 0);
	assert(textbuffer_lines(tb1) == 6);
	assert(textbuffer_bytes(tb1) == 11);
	assert(textbuffer_lines(tb2) == 3);
	assert(textbuffer_bytes(tb2) == 5);
	textbuffer_drop(tb1);
	textbuffer_drop(tb2);

	//paste at the begin
	tb1 = textbuffer_new("1\n2\n3\n");
	tb2 = textbuffer_new("4\n5\n\n");
	textbuffer_paste(tb1, 0, tb2);
	assert(strcmp(textbuffer_to_str(tb1), "4\n5\n\n1\n2\n3\n") == 0);
	assert(strcmp(textbuffer_to_str(tb2), "4\n5\n\n") == 0);
	assert(textbuffer_lines(tb1) == 6);
	assert(textbuffer_bytes(tb1) == 11);
	assert(textbuffer_lines(tb2) == 3);
	assert(textbuffer_bytes(tb2) == 5);
	textbuffer_drop(tb1);
	textbuffer_drop(tb2);

	//paste between
	tb1 = textbuffer_new("1\n2\n3\n");
	tb2 = textbuffer_new("4\n5\n\n");
	textbuffer_paste(tb1, 1, tb2);
	assert(strcmp(textbuffer_to_str(tb1), "1\n4\n5\n\n2\n3\n") == 0);
	assert(strcmp(textbuffer_to_str(tb2), "4\n5\n\n") == 0);
	assert(textbuffer_lines(tb1) == 6);
	assert(textbuffer_bytes(tb1) == 11);
	assert(textbuffer_lines(tb2) == 3);
	assert(textbuffer_bytes(tb2) == 5);
	textbuffer_drop(tb1);
	textbuffer_drop(tb2);

	// paste out of range
	// tb1 = textbuffer_new("1\n2\n3\n");
	// tb2 = textbuffer_new("4\n5\n\n");
	// textbuffer_paste(tb1, 100, tb2);
}
static void test_text_buffer_cut(void)
{
	Textbuffer tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
	Textbuffer tb2 = textbuffer_cut(tb1, 0, textbuffer_lines(tb1) - 1);
	assert(strcmp(textbuffer_to_str(tb1), "") == 0);
	assert(strcmp(textbuffer_to_str(tb2), "1\n2\n3\n4\n5\n6\n") == 0);
	assert(textbuffer_lines(tb1) == 0);
	assert(textbuffer_bytes(tb1) == 0);
	assert(textbuffer_lines(tb2) == 6);
	assert(textbuffer_bytes(tb2) == 12);
	textbuffer_drop(tb1);
	textbuffer_drop(tb2);

	//cut between
	tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
	tb2 = textbuffer_cut(tb1, 1, 3);
	assert(strcmp(textbuffer_to_str(tb1), "1\n5\n6\n") == 0);
	assert(strcmp(textbuffer_to_str(tb2), "2\n3\n4\n") == 0);
	assert(textbuffer_lines(tb1) == 3);
	assert(textbuffer_bytes(tb1) == 6);
	assert(textbuffer_lines(tb2) == 3);
	assert(textbuffer_bytes(tb2) == 6);
	textbuffer_drop(tb1);
	textbuffer_drop(tb2);

	//from > to
	tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
	tb2 = textbuffer_cut(tb1, 3, 1);
	assert(strcmp(textbuffer_to_str(tb1), "1\n2\n3\n4\n5\n6\n") == 0);
	assert(strcmp(textbuffer_to_str(tb2), "") == 0);
	assert(textbuffer_lines(tb1) == 6);
	assert(textbuffer_bytes(tb1) == 12);
	assert(textbuffer_lines(tb2) == 0);
	assert(textbuffer_bytes(tb2) == 0);
	textbuffer_drop(tb1);
	textbuffer_drop(tb2);

	//from outbound
	// tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
	// tb2 = textbuffer_cut(tb1, -1, 1);

	//to outbound
	// tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
	// tb2 = textbuffer_cut(tb1, 0, 6);
}
static void test_text_buffer_copy(void)
{
	 //copy all
    Textbuffer tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    Textbuffer tb2 = textbuffer_copy(tb1, 0, textbuffer_lines(tb1) - 1);
    assert(strcmp(textbuffer_to_str(tb1), "1\n2\n3\n4\n5\n6\n") == 0);
    assert(strcmp(textbuffer_to_str(tb2), "1\n2\n3\n4\n5\n6\n") == 0);
    assert(tb1 != tb2);
    assert(textbuffer_lines(tb1) == 6);
    assert(textbuffer_bytes(tb1) == 12);
    assert(textbuffer_lines(tb2) == 6);
    assert(textbuffer_bytes(tb2) == 12);
    textbuffer_drop(tb1);
    textbuffer_drop(tb2);

    //copy between
    tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    tb2 = textbuffer_copy(tb1, 2, 3);
    assert(strcmp(textbuffer_to_str(tb1), "1\n2\n3\n4\n5\n6\n") == 0);
    assert(strcmp(textbuffer_to_str(tb2), "3\n4\n") == 0);
    assert(textbuffer_lines(tb1) == 6);
    assert(textbuffer_bytes(tb1) == 12);
    assert(textbuffer_lines(tb2) == 2);
    assert(textbuffer_bytes(tb2) == 4);
    textbuffer_drop(tb1);
    textbuffer_drop(tb2);

    //from > to
    tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    tb2 = textbuffer_copy(tb1, 4, 3);
    assert(strcmp(textbuffer_to_str(tb1), "1\n2\n3\n4\n5\n6\n") == 0);
    assert(strcmp(textbuffer_to_str(tb2), "") == 0);
    assert(textbuffer_lines(tb1) == 6);
    assert(textbuffer_bytes(tb1) == 12);
    assert(textbuffer_lines(tb2) == 0);
    assert(textbuffer_bytes(tb2) == 0);
    textbuffer_drop(tb1);
    textbuffer_drop(tb2);

    //from outbound
    // tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // tb2 = textbuffer_copy(tb1, -1, 1);

    //to outbound
    // tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // tb2 = textbuffer_copy(tb1, 2, 7);
}
static void test_text_buffer_delete(void)
{
	//delete all
    Textbuffer tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    textbuffer_delete(tb1, 0, textbuffer_lines(tb1) - 1);
    assert(strcmp(textbuffer_to_str(tb1), "") == 0);
    assert(textbuffer_lines(tb1) == 0);
    assert(textbuffer_bytes(tb1) == 0);
    textbuffer_drop(tb1);

    //delete between
    tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    textbuffer_delete(tb1, 1, 3);
    assert(strcmp(textbuffer_to_str(tb1), "1\n5\n6\n") == 0);
    assert(textbuffer_lines(tb1) == 3);
    assert(textbuffer_bytes(tb1) == 6);
    textbuffer_drop(tb1);

    //from > to
    tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    textbuffer_delete(tb1, 3, 1);
    assert(strcmp(textbuffer_to_str(tb1), "1\n2\n3\n4\n5\n6\n") == 0);
    assert(textbuffer_lines(tb1) == 6);
    assert(textbuffer_bytes(tb1) == 12);
    textbuffer_drop(tb1);

    //from outbound
    // tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // textbuffer_delete(tb1, -1, 1);

    //to outbound
    // tb1 = textbuffer_new("1\n2\n3\n4\n5\n6\n");
    // textbuffer_delete(tb1, 0, 8);
}
static void test_text_buffer_search(void)
{
	//found some line
	Textbuffer tb = textbuffer_new("Hello guy\nGood Guy\nBad guy\ngood\ngguyy\n\n");
    assert(textbuffer_search(tb, "guy", 0) == 3);
    assert(textbuffer_search(tb, "guy", 1) == 3);
    textbuffer_drop(tb);

    //Search by empty string (all lines match)
    tb = textbuffer_new("Hello guy\nGood Guy\nBad guy\ngood\ngguyy\n\n");
    assert(textbuffer_search(tb, "", 0) == (int)(textbuffer_lines(tb)));
    assert(textbuffer_search(tb, "", 1) == (int)(textbuffer_lines(tb)));
    textbuffer_drop(tb);

    //Search not found
    tb = textbuffer_new("Hello guy\nGood Guy\nBad guy\ngood\ngguyy\n\n");
    assert(textbuffer_search(tb, "gag", 0) == -1);
    assert(textbuffer_search(tb, "gag", 1) == -1);
    textbuffer_drop(tb);
}
static void test_text_buffer_replace(void)
{
	 //replace with same length text
    Textbuffer tb = textbuffer_new("Hello world world!!!\nHello world\nGood World\nGood Bye\n");
    textbuffer_replace(tb, "world", "earth");
    assert(strcmp(textbuffer_to_str(tb), "Hello earth earth!!!\nHello earth\nGood World\nGood Bye\n") == 0);
    assert(textbuffer_lines(tb) == 4);
    assert(textbuffer_bytes(tb) == 53);
    textbuffer_drop(tb);

    // //replace with smaller text
    tb = textbuffer_new("Hello world world!!!\nHello world\nGood World\nGood Bye\n");
    textbuffer_replace(tb, "world", "unsw");
    assert(strcmp(textbuffer_to_str(tb), "Hello unsw unsw!!!\nHello unsw\nGood World\nGood Bye\n") == 0);
    assert(textbuffer_lines(tb) == 4);
    assert(textbuffer_bytes(tb) == 50);
    textbuffer_drop(tb);

    //replace with bigger text
    tb = textbuffer_new("Hello world world!!!\nHello world\nGood World\nGood Bye\n");
    textbuffer_replace(tb, "world", "SydneySydneySydney");
    assert(strcmp(textbuffer_to_str(tb), "Hello SydneySydneySydney SydneySydneySydney!!!\nHello SydneySydneySydney\nGood World\nGood Bye\n") == 0);
    assert(textbuffer_lines(tb) == 4);
    assert(textbuffer_bytes(tb) == 92);
    textbuffer_drop(tb);

    // //replace with empty text
    tb = textbuffer_new("Hello world world!!!\nHello world\nGood World\nGood Bye\n");
    textbuffer_replace(tb, "world", "");
    assert(strcmp(textbuffer_to_str(tb), "Hello  !!!\nHello \nGood World\nGood Bye\n") == 0);
    assert(textbuffer_lines(tb) == 4);
    assert(textbuffer_bytes(tb) == 38);
    textbuffer_drop(tb);
}
