#include <stdio.h>
#include <malloc.h>

struct Pair;

class Node
{
    public:

        unsigned int size;

        Pair* pair;
};

union PairContent
{
    char* value;
    Node next;
};

struct Pair
{
    bool isParent;
    char* label;
    union
    {
        char* value;
        Node next;
    };
};

class Parsed
{
    protected:

        char* buffer;

        unsigned int strlen
            (char* str)
        {
            unsigned int len = 0;
            for (int i = 0; str[i] != '\0'; ++i)
            {
                ++len;
            }
            return len;
        }

        bool strcmp
            (char* str1, char* str2)
        {
            if (strlen(str1) != strlen(str2))
            {
                return false;
            }
            for (int i = 0; str1[i] != '\0' || str2[i] != '\0'; ++i)
            {
                if (str1[i] != str2[i])
                {
                    return false;
                }
            }
            return true;
        }

        void skip_to_value
            (char* data, unsigned int& p)
        {
            beginning: ;
            for ( ; data[p] && data[p] <= ' '; ++p)                                         //  Skip whitespace.
            {
            }
            if (data[p] == '/' && data[p + 1] == '/')                                       //  Comment.
            {
                p += 2;
                for ( ; data[p] && data[p] != '\n'; ++p)                                    //  Skip to end of line.
                {  
                }
                ++p;
                goto beginning;                                                             //  Repeat again.
            }
            if (data[p] == '/' && data[p + 1] == '*')                                       //  Block comment.
            {
                p += 2;
                for ( ; data[p] && data[p] != '*' && data[p + 1] != '\\'; ++p)              //  Skip to end of block comment.
                { 
                }
                p += 2;
                goto beginning;                                                             //  Repeat again.
            }
        }

        char* parse_string(char* data, unsigned int& p)
        {
            char* string;
            if (data[p] == '\"')
            {
                ++p;
                string = data + p;                                                          //  Start the string.
                for ( ; data[p] != '\"'; ++p)                                               //  Skip to quote.
                {
                }
            }
            else
            {
                string = data + p;
                for ( ; data[p] >= ' '; ++p)                                                //  Skip anything that isnt whitespace.
                {
                }
            }
            data[p] = '\0';                                                                 //  End the string.
            ++p;                                                                            //  Increment the pointer after finishing.
            return string;
        }

        Node parse_data
            (char* data, unsigned int& p, unsigned int size)
        {
            Node newNode = {0, NULL};                                                       //  Starting object. The magic begins.
            unsigned int alloc = 1;                                                         //  Allocate one object. With the magic of realloc, we can start at 1.
            newNode.pair = (Pair* )calloc(alloc, sizeof(Pair));                             //  Allocate those pointers.
            skip_to_value(data, p);                                                         //  Skips whitespace.

            for ( ; p < size; )
            {
                if (data[p] == '}')                                                         //  End of object? Nice, return it.
                {
                    ++p;                                                                    //  Increment the pointer so we don't read curly brackets as strings.
                    return newNode;
                }
                if (newNode.size >= alloc)                                                  //  We got more than one label and value? Pog, allocate some more.
                {
                    ++alloc;
                    newNode.pair = (Pair* )realloc(newNode.pair, alloc * sizeof(Pair));
                }
                char* label = parse_string(data, p);                                        //  Read our label, the filepointer should be set right onto a label.
                skip_to_value(data, p);                                                     //  Skip to our value/next object.
                PairContent pair;                                                           //  Union that either has a value or a new object.
                if (data[p] == '{')                                                         //  New object? Poggers.
                {
                    pair.next = parse_data(data, ++p, size);                                //  Recurse.
                    newNode.pair[newNode.size].next = pair.next;                            
                    newNode.pair[newNode.size].isParent = true;                             //  This is to help with printing the structure later.
                }
                else
                {
                    pair.value = parse_string(data, p);                                     //  Boring value, not poggers.
                    newNode.pair[newNode.size].value = pair.value;
                    newNode.pair[newNode.size].isParent = false;
                }

                newNode.pair[newNode.size].label = label;                                   //  Set our label.
                ++newNode.size;                                                             //  Get ready for a new object.
                skip_to_value(data, p);
            }
            return newNode;                                                                 //  Success, the file is parsed.
        }

        void print_dump
            (Node nodeStart, unsigned int iterations)
        {
            for (int i = 0; i < nodeStart.size; ++i)
            {
                if (nodeStart.pair[i].isParent)
                {
                    for (int j = 0; j < iterations; ++j)
                        printf("\t");
                    printf("\"%s\"\n", nodeStart.pair[i].label);
                    for (int j = 0; j < iterations; ++j)
                        printf("\t");
                    printf("{\t//\t%i sub-items\n", nodeStart.size + 1);
                    print_dump(nodeStart.pair[i].next, ++iterations);
                    for (int j = 0; j < iterations - 1; ++j)
                        printf("\t");
                    printf("}\n");
                    --iterations;
                }
                else
                {
                    for (int j = 0; j < iterations; ++j)
                        printf("\t");
                    printf("\"%s\" \"%s\"\n", nodeStart.pair[i].label, nodeStart.pair[i].value);
                }
            }
        }

        void free_parsed
            (Node nodeStart)
        {
            for (int i = 0; i < nodeStart.size; ++i)
            {
                if (nodeStart.pair[i].isParent)
                {
                    free_parsed(nodeStart.pair[i].next);
                }
            }
            free(nodeStart.pair);
        }

    public:

        Node nodeHead;                                                                      //  Contains our parsed data.

        char* get_value_from_label_in_object
            (Node nodeStart, const char* objectLabel, const char* itemLabel)
        {
            char* value = NULL;
            for (int i = 0; i < nodeStart.size; ++i)
            {
                if (strcmp(nodeStart.pair[i].label, (char*)objectLabel) && nodeStart.pair[i].isParent)
                {
                    for (int j = 0; j < nodeStart.pair[i].next.size; ++j)
                    {
                        if (strcmp(nodeStart.pair[i].next.pair[j].label, (char*)itemLabel))
                        {
                            return nodeStart.pair[i].next.pair[j].value;
                        }
                    }
                }
                else if (nodeStart.pair[i].isParent)
                {
                    value = get_value_from_label_in_object(nodeStart.pair[i].next, objectLabel, itemLabel);
                    if (value)
                    {
                        return value;
                    }
                }
            }
            return NULL;
        }

        void dump
            (Node nodeStart)
        {
            unsigned int i = 0;
            print_dump(nodeStart, i);
        }

        Parsed
            (const char* path)
        {
            FILE *f = fopen(path, "r");                                                     //  Open our file.
            unsigned int p = 0;                                                             //  Make a pointer for our filebuffer.
            unsigned int size = 0;                                                          //  Filesize.

            fseek(f, 0, SEEK_END);
            size = ftell(f);                                                                //  Get filesize.
            rewind(f);
            buffer = (char*)malloc(size);
            fread(buffer, size, 1, f);
            fclose(f);

            nodeHead = parse_data(buffer, p, size);                                         //  Let the magic start.
        }
        ~Parsed
            ()
        {
            free_parsed(nodeHead);
            free(buffer);
        }
};
