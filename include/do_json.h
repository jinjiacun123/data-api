#ifndef __DO_JSON_H__
#define __DO_JSON_H__
extern unsigned long get_c_request_package_length(char *);
extern unsigned long format_json_to_client(server_package_t );
extern int parse_client_request(char *, cJSON *, char *);

extern char * json_get_string(cJSON * json, char * name, char * value);
extern int    json_get_int(cJSON * json, char * name);
extern float  json_get_float(cJSON * json, char *name);

#endif
