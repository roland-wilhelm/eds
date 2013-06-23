#ifdef LOG_H_
#define LOG_H_


#define DEBUG
#ifdef DEBUG
#define DBG(fmt, args...) printf("%s: %s:%d: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ## args);;
#else
#define DBG(fmt, args...)
#endif


#endif
