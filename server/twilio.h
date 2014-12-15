#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

struct url_data {
    size_t size;
    char* data;
};

size_t write_data(void*, size_t, size_t, struct url_data *);
char *sendSMS(char* body, char* to);