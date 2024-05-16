/**
 * Platform / input options
 */
#define SHOCKS_FROM_HEADER		1
#define ACCEL					1
#define ACCEL_SETUP				1
#define SMALL_PL				0
#define SMALL_PL_MAX_ACCEL		0

#if !defined(__SDSCC__) // is there any need to tie BARE_METAL to SDSCC?
	#define BARE_METAL			0
#else
	#define BARE_METAL			1
#endif

/**
 * Debug interfaces
 */
#define OUT_DEBUG_DATA			0
#define OUTPUT_KP				0
#define VERBOSE					0

/**
 * Code refactoring options
 */
#define EGM_ONE_LOOP			1

/**
 * Operations options
 */
#define USE_EXP_AS_POW			0

/**
 * Detailed acceleration options
 */
#define AST_UNROLL				1

/**
 * Experiments
 *	Move to another section when mature
 */
#define OLD_INLINE_2D			1
#define SPREAD_SCALAR_TRICK		0
#define IDS_AGG_PACK			0 // If 1, AST_UNROLL must be 1?
#if IDS_AGG_PACK
#define IDS_AGG_X				1 // bytes packed into ids_hw_t
#else
#define IDS_AGG_X				1
#endif
#define NUM_KCROSS				(8 * IDS_AGG_X)

/**
 * Data precision
 */
#define PRECISION_FLOAT			1 // Overrides fixed
#define PRECISION_FIXED			0 // "all" ('real') fixed
#define PRECISION_EXTRA_FIXED	1 // Selective additional use of ap_fixed
// some of the above should be moved into standard FPGA synthesis

/**
 * Precision check output files
 */
#if !PRECISION_FLOAT
#define KP_OUT_FILE			"./kp_out/double/kpo_"
#elif PRECISION_FIXED
#define KP_OUT_FILE			"./kp_out/allfixed/kpo_"
#elif PRECISION_EXTRA_FIXED
#define KP_OUT_FILE			"./kp_out/fixed/kpo_"
#else
#define KP_OUT_FILE			"./kp_out/float/kpo_"
#endif

// I have written powf so I can't even do the double test. Sigh.
