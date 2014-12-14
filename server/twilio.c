#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "twilio.h"

size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;

    data->size += (size * nmemb);

#ifdef DEBUG
    fprintf(stderr, "data at %p size=%ld nmemb=%ld\n", ptr, size, nmemb);
#endif
    tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

    if(tmp) {
        data->data = tmp;
    } else {
        if(data->data) {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    return size * nmemb;
}

char *sendSMS(char* body, char* to) {
    CURL *curl;

    struct url_data data;
    data.size = 0;
    data.data = malloc(4096); /* reasonable size initial buffer */
    if(NULL == data.data) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    data.data[0] = '\0';

    CURLcode res;

    char jsonObj[150];
      
    curl = curl_easy_init();
    if (curl) {
        char *output = curl_easy_escape(curl, body, strlen(body));
        if(output) {
            sprintf(jsonObj, "&Body=%s&To=%2B1%s&From=%2B112342001902", output, to);
        }  else {
            printf("Failed to encode the text");
        }
    // set cURL options
        curl_easy_setopt( curl, CURLOPT_POST, 3 ); // number of fields
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.twilio.com/2010-04-01/Accounts/ACcf028faf52ac63399b3345c6c29203ff/Messages.json");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonObj);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(curl, CURLOPT_USERPWD, "ACcf028faf52ac63399b3345c6c29203ff:e2361f5debf2e1bb7892c32534dd79a2");
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",  
                        curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);

    }
    return data.data;
}
