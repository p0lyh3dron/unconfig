#include "parser.hpp"

int main
    (void)
{
    Parsed p("exampleconfig.cfg");

    printf("a is %s\n", p.get_value_from_label_in_object(p.nodeHead, "values", "a"));
    printf("b is %s\n", p.get_value_from_label_in_object(p.nodeHead, "values", "b"));
    printf("c is %s\n", p.get_value_from_label_in_object(p.nodeHead, "values", "c"));

    return 0;
}