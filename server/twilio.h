struct url_data {
    size_t size;
    char* data;
};

size_t write_data(void*, size_t, size_t, struct url_data *);
char *sendSMS(char*);