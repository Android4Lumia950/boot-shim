#pragma once

// Define the ARM SMC Arguments structure
typedef struct {
    UINTN Arg0;
    UINTN Arg1;
    UINTN Arg2;
    UINTN Arg3;
    UINTN Arg4;
    UINTN Arg5;
    UINTN Arg6;
    UINTN Arg7;
} ARM_SMC_ARGS;

// Forward declaration for function
extern VOID ArmCallSmcHardCoded(VOID);

// Qualcomm SCM definitions
#define SCM_SVC_MILESTONE_32_64_ID 0x1
#define SCM_SVC_MILESTONE_CMD_ID   0xf
#define SIP_SVC_CALLS              0x02000000
#define SCM_MAX_ARG_LEN            5
#define SCM_INDIR_MAX_LEN          10

// SMC parameter types
typedef enum {
    SMC_PARAM_TYPE_VALUE = 0,
    SMC_PARAM_TYPE_BUFFER_READ,
    SMC_PARAM_TYPE_BUFFER_READWRITE,
    SMC_PARAM_TYPE_BUFFER_VALIDATION,
} scm_arg_type;

// Macro to create SIP SCM command
#define MAKE_SIP_SCM_CMD(svc_id, cmd_id) \
    ((((svc_id) << 8 | (cmd_id)) & 0xFFFF) | SIP_SVC_CALLS)

// Macro to create SCM variable arguments
#define MAKE_SCM_VAR_ARGS(num_args, ...) \
    (((((num_args) & 0xFFFF) << 0) | \
    (((__VA_ARGS__) & 0xFF) << 4) | \
    ((((num_args) >= 2) ? (((__VA_ARGS__ >> 8) & 0xFF) << 6) : 0)) | \
    ((((num_args) >= 3) ? (((__VA_ARGS__ >> 16) & 0xFF) << 8) : 0)) | \
    ((((num_args) >= 4) ? (((__VA_ARGS__ >> 24) & 0xFF) << 10) : 0)) | \
    ((((num_args) >= 5) ? (((__VA_ARGS__ >> 32) & 0xFF) << 12) : 0)) | \
    ((((num_args) >= 6) ? (((__VA_ARGS__ >> 40) & 0xFF) << 14) : 0)) | \
    ((((num_args) >= 7) ? (((__VA_ARGS__ >> 48) & 0xFF) << 16) : 0)) | \
    ((((num_args) >= 8) ? (((__VA_ARGS__ >> 56) & 0xFF) << 18) : 0)) | \
    ((((num_args) >= 9) ? (((__VA_ARGS__ >> 64) & 0xFF) << 20) : 0)))

// Macro to create SCM arguments
#define MAKE_SCM_ARGS(...) MAKE_SCM_VAR_ARGS(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

// Macro to calculate bit shifts
#define BIT(x) (1 << (x))
#define SCM_ATOMIC_BIT BIT(31)

// Structure for SCM call arguments
typedef struct {
    uint32_t x0; // Command ID and attributes
    uint32_t x1; // Number of args and attributes
    uint32_t x2; // Param1
    uint32_t x3; // Param2
    uint32_t x4; // Param3
    uint32_t x5[10]; // Indirect parameters
    uint32_t atomic; // Indicates standard or fast call
} scmcall_arg;

// Structure for SCM call return values
typedef struct {
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
} scmcall_ret;

// Structure for EL1 system parameters
typedef struct {
    uint64_t el1_x0;
    uint64_t el1_x1;
    uint64_t el1_x2;
    uint64_t el1_x3;
    uint64_t el1_x4;
    uint64_t el1_x5;
    uint64_t el1_x6;
    uint64_t el1_x7;
    uint64_t el1_x8;
    uint64_t el1_elr;
} el1_system_param;

// Utility macros for alignment
#define ROUNDUP(a, b) (((a) + ((b) - 1)) & ~((b) - 1))
#define ROUNDDOWN(a, b) ((a) & ~((b) - 1))

#endif // __EDK2_SCMLIB_H__
