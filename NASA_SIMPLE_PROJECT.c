/*
 * NASA-Grade Macro-Driven C Single File
 * ------------------------------------------------------------------
 * This single translation unit demonstrates professional, mission-critical
 * macro usage patterns: compile-time config, platform abstraction,
 * multi-statement macros, X-macros for codegen, safety checks, logging,
 * likely/unlikely hints, compile-time asserts, and (simulated) HW registers.
 *
 * Build examples:
 *   Ground (debug):
 *     gcc -std=c11 -O2 -DGROUND_BUILD -DDEBUG -DSIM_HW_REGS nasa_macro.c -o nasa_macro
 *
 *   Flight (optimized, no debug logs):
 *     gcc -std=c11 -O2 -DNDEBUG -DFLIGHT_BUILD -DSIM_HW_REGS nasa_macro.c -o nasa_macro
 *
 *   Architecture switch (choose exactly one):
 *     -DCPU_ARM   or   -DCPU_RISCV
 *
 * Run:
 *   ./nasa_macro
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------------
 * 0) Safety: mutually-exclusive build types and arch selection
 * ------------------------------------------------------------------ */
#if defined(FLIGHT_BUILD) && defined(GROUND_BUILD)
#  error "Choose only one: FLIGHT_BUILD or GROUND_BUILD"
#endif
#if !defined(FLIGHT_BUILD) && !defined(GROUND_BUILD)
#  warning "No build type provided; defaulting to GROUND_BUILD"
#  define GROUND_BUILD 1
#endif

#if !defined(CPU_ARM) && !defined(CPU_RISCV)
#  warning "No CPU defined; defaulting to CPU_RISCV"
#  define CPU_RISCV 1
#endif

/* ------------------------------------------------------------------
 * 1) Compile-time configuration knobs
 * ------------------------------------------------------------------ */
#if defined(FLIGHT_BUILD)
#  define CFG_ENABLE_ASSERTS   0  /* minimal runtime checks */
#  define CFG_ENABLE_LOGS      0  /* logs stripped */
#  define CFG_MAX_THRUST_N     5000
#elif defined(GROUND_BUILD)
#  define CFG_ENABLE_ASSERTS   1
#  define CFG_ENABLE_LOGS      1
#  define CFG_MAX_THRUST_N     4000
#endif

/* Safety gate: prevent unsafe thrust in current spacecraft config */
#if CFG_MAX_THRUST_N > 6000
#  error "CFG_MAX_THRUST_N exceeds structural limit"
#endif

/* ------------------------------------------------------------------
 * 2) Compile-time assertions (portable)
 * ------------------------------------------------------------------ */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define STATIC_ASSERT(expr, msg) _Static_assert((expr), #msg)
#else
#  define STATIC_ASSERT(expr, msg) typedef char static_assert_##msg[(expr)?1:-1]
#endif

STATIC_ASSERT(sizeof(void*) >= 4, pointer_size_must_be_32b_or_more);

/* ------------------------------------------------------------------
 * 3) Branch prediction and attributes (portable wrappers)
 * ------------------------------------------------------------------ */
#if defined(__GNUC__) || defined(__clang__)
#  define LIKELY(x)   __builtin_expect(!!(x), 1)
#  define UNLIKELY(x) __builtin_expect(!!(x), 0)
#  define NOINLINE    __attribute__((noinline))
#else
#  define LIKELY(x)   (x)
#  define UNLIKELY(x) (x)
#  define NOINLINE
#endif

/* ------------------------------------------------------------------
 * 4) Logging and tracing (zero-cost in flight builds)
 * ------------------------------------------------------------------ */
#if CFG_ENABLE_LOGS
#  define LOGF(fmt, ...) \
      fprintf(stderr, "[LOG] %s:%d %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#  define TRACE() \
      fprintf(stderr, "[TRACE] %s:%d in %s()\n", __FILE__, __LINE__, __func__)
#else
#  define LOGF(fmt, ...) ((void)0)
#  define TRACE()        ((void)0)
#endif

/* ------------------------------------------------------------------
 * 5) Multi-statement macros (statement-safe)
 * ------------------------------------------------------------------ */
#define SCOPE_DO(block) do { block } while (0)

#define SAFE_CALL(call) \
    SCOPE_DO({ \
        int _rc = (call); \
        if (UNLIKELY(_rc != 0)) { \
            LOGF("SAFE_CALL failed: %s -> rc=%d", #call, _rc); \
            exit(EXIT_FAILURE); \
        } \
    })

/* Simple runtime assert gated by config */
#if CFG_ENABLE_ASSERTS
#  include <assert.h>
#  define ASSERT(x) SCOPE_DO({ if(!(x)){ LOGF("ASSERT: %s", #x); assert(x); } })
#else
#  define ASSERT(x) ((void)0)
#endif

/* ------------------------------------------------------------------
 * 6) Hardware registers (simulated mapping)
 *    In flight, these would be real memory-mapped addresses. Here we
 *    simulate with a struct so the demo runs on any host.
 * ------------------------------------------------------------------ */
#ifdef SIM_HW_REGS
typedef struct {
    volatile uint32_t CTRL;      /* control register */
    volatile uint32_t STATUS;    /* status register  */
    volatile uint32_t THRUST;    /* thrust (Newtons) */
    volatile uint32_t SENS_TEMP; /* temp sensor (raw)*/
} hw_regs_t;
static hw_regs_t HW = {0};
#  define REG32(name)     (HW.name)
#else
#  define REG32_PTR(addr) (*(volatile uint32_t*)(addr))
#  define REG32(name)     REG32_PTR(name)
#endif

/* Bit fields for CTRL */
#define CTRL_ENABLE   (1u<<0)
#define CTRL_FAULT    (1u<<1)

#define ENABLE_SYSTEM()  SCOPE_DO({ REG32(CTRL) |= CTRL_ENABLE;  LOGF("System ENABLED"); })
#define DISABLE_SYSTEM() SCOPE_DO({ REG32(CTRL) &= ~CTRL_ENABLE; LOGF("System DISABLED"); })
#define SIGNAL_FAULT()   SCOPE_DO({ REG32(CTRL) |= CTRL_FAULT;   LOGF("FAULT signaled"); })

/* Thrust write with safety cap */
#define SET_THRUST_N(newton) \
    SCOPE_DO({ \
        uint32_t _n = (uint32_t)(newton); \
        if (UNLIKELY(_n > CFG_MAX_THRUST_N)) { \
            LOGF("Thrust request %u exceeds limit %u — capping", _n, (unsigned)CFG_MAX_THRUST_N); \
            _n = CFG_MAX_THRUST_N; \
        } \
        REG32(THRUST) = _n; \
        LOGF("THRUST set to %u N", _n); \
    })

/* ------------------------------------------------------------------
 * 7) Platform/arch abstraction (sensor read)
 * ------------------------------------------------------------------ */
static inline int read_sensor_riscv(int *out) { *out = (int)REG32(SENS_TEMP); return 0; }
static inline int read_sensor_arm  (int *out) { *out = (int)REG32(SENS_TEMP); return 0; }

#if defined(CPU_ARM)
#  define READ_TEMP_RAW(ptr_int) read_sensor_arm(ptr_int)
#elif defined(CPU_RISCV)
#  define READ_TEMP_RAW(ptr_int) read_sensor_riscv(ptr_int)
#endif

/* ------------------------------------------------------------------
 * 8) X-macros: error codes and messages kept in sync
 * ------------------------------------------------------------------ */
#define ERROR_TABLE \
    X(ERR_OK,            0,  "No Error") \
    X(ERR_SENSOR_FAIL,   10, "Sensor Failure") \
    X(ERR_THRUST_RANGE,  20, "Thrust Out of Range") \
    X(ERR_SYSTEM_FAULT,  30, "System Fault")

typedef enum {
#define X(name, code, msg) name = code,
    ERROR_TABLE
#undef X
} error_t;

static const char* error_to_str(error_t e) {
    switch (e) {
#define X(name, code, msg) case name: return msg;
        ERROR_TABLE
#undef X
        default: return "Unknown";
    }
}

/* ------------------------------------------------------------------
 * 9) Tiny guidance/control demo using the macros above
 * ------------------------------------------------------------------ */
static NOINLINE int init_system(void) {
    TRACE();
    ENABLE_SYSTEM();
    REG32(SENS_TEMP) = 42; /* seed */
    return 0; /* simulate success */
}

static NOINLINE int poll_temperature_c(int *out_c) {
    TRACE();
    int raw = 0;
    if (READ_TEMP_RAW(&raw) != 0) return ERR_SENSOR_FAIL;
    /* Simple linearization: raw -> Celsius */
    *out_c = raw; /* pretend already degC */
    LOGF("Temp=%d C", *out_c);
    return ERR_OK;
}

static NOINLINE int command_thrust(uint32_t desired_n) {
    TRACE();
    if (desired_n > (CFG_MAX_THRUST_N * 2)) {
        return ERR_THRUST_RANGE; /* clearly insane input */
    }
    SET_THRUST_N(desired_n);
    return ERR_OK;
}

static NOINLINE int run_control_loop_once(void) {
    TRACE();
    int temp_c = 0;
    int rc = poll_temperature_c(&temp_c);
    if (rc != ERR_OK) return rc;

    /* Simple policy: map temperature to thrust */
    uint32_t desired = (temp_c < 30) ? 3000 : 1500; // demo policy
    rc = command_thrust(desired);
    if (rc != ERR_OK) return rc;

    if (UNLIKELY(REG32(CTRL) & CTRL_FAULT)) {
        return ERR_SYSTEM_FAULT;
    }
    return ERR_OK;
}

/* ------------------------------------------------------------------
 * 10) Main: tie it together
 * ------------------------------------------------------------------ */
int main(void) {
    LOGF("Build: %s %s | C%ld | Hosted=%d",
         __DATE__, __TIME__, (long)__STDC_VERSION__, (int)__STDC_HOSTED__);

    SAFE_CALL(init_system());

    /* Self-checks */
    ASSERT((CFG_MAX_THRUST_N % 10) == 0);

    /* Demo loop */
    for (int i = 0; i < 3; ++i) {
        int rc = run_control_loop_once();
        if (rc != ERR_OK) {
            fprintf(stderr, "ERROR %d: %s\n", rc, error_to_str(rc));
            break;
        }
        /* Mutate temp to exercise policy */
        REG32(SENS_TEMP) += 5;
    }

    /* Exercise fault path */
    SIGNAL_FAULT();
    int rc = run_control_loop_once();
    if (rc != ERR_OK) {
        fprintf(stderr, "ERROR %d: %s\n", rc, error_to_str(rc));
    }

    DISABLE_SYSTEM();
    return 0;
}
