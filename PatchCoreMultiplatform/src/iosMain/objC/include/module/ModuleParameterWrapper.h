#ifndef MODULE_PARAMETER_WRAPPER_H
#define MODULE_PARAMETER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    int type; // 0 = int, 1 = float, 2 = bool
    int enumValue;
    float floatValue;
    int boolValue;
    char* key;
} ModuleParameterWrapper;

#ifdef __cplusplus
}
#endif

#endif // MODULE_PARAMETER_WRAPPER_H

