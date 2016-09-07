#ifndef __DO_JSON_H__
#define __DO_JSON_H__
extern unsigned long get_c_request_package_length(char *);
extern unsigned long format_json_to_client(cJSON *, char *, char *);
extern int parse_client_request(char *, cJSON *, char *);
#endif
