#include "parser.h"

struct Node;
struct Parsed;

int main
    (void)
{
    struct Parsed p;

    p = parse("exampleconfig.cfg");

    printf("highest value is %s\n\n", get_value_from_label_in_object(p.nodeHead, "value information", "highest number")); //  Read the object in the object.

    printf("a is %s\n", get_value_from_label_in_object(p.nodeHead, "values", "a"));   //  Find label a and get its value.
    printf("b is %s\n", get_value_from_label_in_object(p.nodeHead, "values", "b"));   //  Find label b and get its value.
    printf("c is %s\n\n", get_value_from_label_in_object(p.nodeHead, "values", "c"));   //  Find label c and get its value.

    dump_parsed(p.nodeHead);
    free_parsed(p);

    return 0;
}