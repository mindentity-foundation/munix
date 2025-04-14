#ifndef __VOIKER_KERNEL_MISC
#define __VOIKER_KERNEL_MISC

void kput(const char* string);
void kputf(const char* format, ...);

#define klog(string) kput(("voiker: "string))
#define klogf(string, ...) kputf(("voiker: "string), __VA_ARGS__)
#define slog(com, string) sput(com, ("voiker: "string))
#define slogf(com, string, ...) sputf(com, ("voiker: "string), __VA_ARGS__)

#endif
