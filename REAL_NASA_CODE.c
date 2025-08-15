/*
 * MACRO: SAFE_CALL
 * Purpose:
 *   Calls a function, checks return code, and exits on failure.
 * Usage:
 *   SAFE_CALL(init_sensor());
 * Constraints:
 *   Must be safe for use in if/else blocks without braces.
 */
#define SAFE_CALL(call) \
    do { \
        int rc = (call); \
        if (rc != 0) { \
            fprintf(stderr, "[%s:%d] %s failed with code %d\n", \
                    __FILE__, __LINE__, #call, rc); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
