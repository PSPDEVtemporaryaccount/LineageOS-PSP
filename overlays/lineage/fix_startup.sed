/^int setFileDefaultsInt(/,/^}/c\
int setFileDefaultsInt(char *path, int value, int var)\
{\
    FILE *temp;\
    if (!fileExists(path)) {\
        temp = fopen(path, "w");\
        if (!temp) return value;\
        fprintf(temp, "%d", value);\
        fclose(temp);\
    }\
    temp = fopen(path, "r");\
    if (!temp) return value;\
    if (fscanf(temp, "%d", &var) != 1) var = value;\
    fclose(temp);\
    return var;\
}
/^float setFileDefaultsFloat(/,/^}/c\
float setFileDefaultsFloat(char *path, float value, float var)\
{\
    FILE *temp;\
    if (!fileExists(path)) {\
        temp = fopen(path, "w");\
        if (!temp) return value;\
        fprintf(temp, "%f", value);\
        fclose(temp);\
    }\
    temp = fopen(path, "r");\
    if (!temp) return value;\
    if (fscanf(temp, "%f", &var) != 1) var = value;\
    fclose(temp);\
    return var;\
}
/^char \* setFileDefaultsChar(/,/^}/c\
char * setFileDefaultsChar(char path[], char data[], char var[])\
{\
    FILE *temp;\
    if (!fileExists(path)) {\
        temp = fopen(path, "w");\
        if (!temp) {\
            strcpy(var, data);\
            return var;\
        }\
        fprintf(temp, "%s", data);\
        fclose(temp);\
    }\
    temp = fopen(path, "r");\
    if (!temp) {\
        strcpy(var, data);\
        return var;\
    }\
    if (fscanf(temp, "%255s", var) != 1) strcpy(var, data);\
    fclose(temp);\
    return var;\
}
