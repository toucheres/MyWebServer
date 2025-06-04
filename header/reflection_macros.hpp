#ifndef REFLECTION_MACROS_HPP
#define REFLECTION_MACROS_HPP

// 0号特殊处理
#define VISIT_CASE_0 \
    if constexpr (N == 0) \
    { \
        /* 无成员，不需要处理 */ \
    }

#define VISIT_CASE_1 \
    else if constexpr (N == 1) \
    { \
        auto&& [a1] = bevisited; \
        visit_helper(a1); \
    }

#define VISIT_CASE_2 \
    else if constexpr (N == 2) \
    { \
        auto&& [a1, a2] = bevisited; \
        visit_helper(a1, a2); \
    }

#define VISIT_CASE_3 \
    else if constexpr (N == 3) \
    { \
        auto&& [a1, a2, a3] = bevisited; \
        visit_helper(a1, a2, a3); \
    }

#define VISIT_CASE_4 \
    else if constexpr (N == 4) \
    { \
        auto&& [a1, a2, a3, a4] = bevisited; \
        visit_helper(a1, a2, a3, a4); \
    }

#define VISIT_CASE_5 \
    else if constexpr (N == 5) \
    { \
        auto&& [a1, a2, a3, a4, a5] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5); \
    }

#define VISIT_CASE_6 \
    else if constexpr (N == 6) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6); \
    }

#define VISIT_CASE_7 \
    else if constexpr (N == 7) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7); \
    }

#define VISIT_CASE_8 \
    else if constexpr (N == 8) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8); \
    }

#define VISIT_CASE_9 \
    else if constexpr (N == 9) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9); \
    }

#define VISIT_CASE_10 \
    else if constexpr (N == 10) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); \
    }

#define VISIT_CASE_11 \
    else if constexpr (N == 11) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11); \
    }

#define VISIT_CASE_12 \
    else if constexpr (N == 12) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12); \
    }

#define VISIT_CASE_13 \
    else if constexpr (N == 13) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13); \
    }

#define VISIT_CASE_14 \
    else if constexpr (N == 14) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14); \
    }

#define VISIT_CASE_15 \
    else if constexpr (N == 15) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15); \
    }

#define VISIT_CASE_16 \
    else if constexpr (N == 16) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16); \
    }

#define VISIT_CASE_17 \
    else if constexpr (N == 17) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17); \
    }

#define VISIT_CASE_18 \
    else if constexpr (N == 18) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18); \
    }

#define VISIT_CASE_19 \
    else if constexpr (N == 19) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19); \
    }

#define VISIT_CASE_20 \
    else if constexpr (N == 20) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20); \
    }

#define VISIT_CASE_21 \
    else if constexpr (N == 21) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21); \
    }

#define VISIT_CASE_22 \
    else if constexpr (N == 22) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22); \
    }

#define VISIT_CASE_23 \
    else if constexpr (N == 23) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23); \
    }

#define VISIT_CASE_24 \
    else if constexpr (N == 24) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24); \
    }

#define VISIT_CASE_25 \
    else if constexpr (N == 25) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25); \
    }

#define VISIT_CASE_26 \
    else if constexpr (N == 26) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26); \
    }

#define VISIT_CASE_27 \
    else if constexpr (N == 27) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27); \
    }

#define VISIT_CASE_28 \
    else if constexpr (N == 28) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28); \
    }

#define VISIT_CASE_29 \
    else if constexpr (N == 29) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29); \
    }

#define VISIT_CASE_30 \
    else if constexpr (N == 30) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30); \
    }

#define VISIT_CASE_31 \
    else if constexpr (N == 31) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31); \
    }

#define VISIT_CASE_32 \
    else if constexpr (N == 32) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32); \
    }

#define VISIT_CASE_33 \
    else if constexpr (N == 33) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33); \
    }

#define VISIT_CASE_34 \
    else if constexpr (N == 34) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34); \
    }

#define VISIT_CASE_35 \
    else if constexpr (N == 35) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35); \
    }

#define VISIT_CASE_36 \
    else if constexpr (N == 36) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36); \
    }

#define VISIT_CASE_37 \
    else if constexpr (N == 37) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37); \
    }

#define VISIT_CASE_38 \
    else if constexpr (N == 38) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38); \
    }

#define VISIT_CASE_39 \
    else if constexpr (N == 39) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39); \
    }

#define VISIT_CASE_40 \
    else if constexpr (N == 40) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40); \
    }

#define VISIT_CASE_41 \
    else if constexpr (N == 41) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41); \
    }

#define VISIT_CASE_42 \
    else if constexpr (N == 42) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42); \
    }

#define VISIT_CASE_43 \
    else if constexpr (N == 43) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43); \
    }

#define VISIT_CASE_44 \
    else if constexpr (N == 44) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44); \
    }

#define VISIT_CASE_45 \
    else if constexpr (N == 45) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45); \
    }

#define VISIT_CASE_46 \
    else if constexpr (N == 46) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46); \
    }

#define VISIT_CASE_47 \
    else if constexpr (N == 47) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47); \
    }

#define VISIT_CASE_48 \
    else if constexpr (N == 48) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48); \
    }

#define VISIT_CASE_49 \
    else if constexpr (N == 49) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49); \
    }

#define VISIT_CASE_50 \
    else if constexpr (N == 50) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50); \
    }

#define VISIT_CASE_51 \
    else if constexpr (N == 51) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51); \
    }

#define VISIT_CASE_52 \
    else if constexpr (N == 52) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52); \
    }

#define VISIT_CASE_53 \
    else if constexpr (N == 53) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53); \
    }

#define VISIT_CASE_54 \
    else if constexpr (N == 54) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54); \
    }

#define VISIT_CASE_55 \
    else if constexpr (N == 55) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55); \
    }

#define VISIT_CASE_56 \
    else if constexpr (N == 56) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56); \
    }

#define VISIT_CASE_57 \
    else if constexpr (N == 57) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57); \
    }

#define VISIT_CASE_58 \
    else if constexpr (N == 58) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58); \
    }

#define VISIT_CASE_59 \
    else if constexpr (N == 59) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59); \
    }

#define VISIT_CASE_60 \
    else if constexpr (N == 60) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60); \
    }

#define VISIT_CASE_61 \
    else if constexpr (N == 61) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61); \
    }

#define VISIT_CASE_62 \
    else if constexpr (N == 62) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62); \
    }

#define VISIT_CASE_63 \
    else if constexpr (N == 63) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63); \
    }

#define VISIT_CASE_64 \
    else if constexpr (N == 64) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64); \
    }

#define VISIT_CASE_65 \
    else if constexpr (N == 65) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65); \
    }

#define VISIT_CASE_66 \
    else if constexpr (N == 66) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66); \
    }

#define VISIT_CASE_67 \
    else if constexpr (N == 67) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67); \
    }

#define VISIT_CASE_68 \
    else if constexpr (N == 68) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68); \
    }

#define VISIT_CASE_69 \
    else if constexpr (N == 69) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69); \
    }

#define VISIT_CASE_70 \
    else if constexpr (N == 70) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70); \
    }

#define VISIT_CASE_71 \
    else if constexpr (N == 71) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71); \
    }

#define VISIT_CASE_72 \
    else if constexpr (N == 72) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72); \
    }

#define VISIT_CASE_73 \
    else if constexpr (N == 73) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73); \
    }

#define VISIT_CASE_74 \
    else if constexpr (N == 74) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74); \
    }

#define VISIT_CASE_75 \
    else if constexpr (N == 75) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75); \
    }

#define VISIT_CASE_76 \
    else if constexpr (N == 76) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76); \
    }

#define VISIT_CASE_77 \
    else if constexpr (N == 77) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77); \
    }

#define VISIT_CASE_78 \
    else if constexpr (N == 78) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78); \
    }

#define VISIT_CASE_79 \
    else if constexpr (N == 79) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79); \
    }

#define VISIT_CASE_80 \
    else if constexpr (N == 80) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80); \
    }

#define VISIT_CASE_81 \
    else if constexpr (N == 81) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81); \
    }

#define VISIT_CASE_82 \
    else if constexpr (N == 82) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82); \
    }

#define VISIT_CASE_83 \
    else if constexpr (N == 83) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83); \
    }

#define VISIT_CASE_84 \
    else if constexpr (N == 84) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84); \
    }

#define VISIT_CASE_85 \
    else if constexpr (N == 85) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85); \
    }

#define VISIT_CASE_86 \
    else if constexpr (N == 86) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86); \
    }

#define VISIT_CASE_87 \
    else if constexpr (N == 87) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87); \
    }

#define VISIT_CASE_88 \
    else if constexpr (N == 88) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88); \
    }

#define VISIT_CASE_89 \
    else if constexpr (N == 89) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89); \
    }

#define VISIT_CASE_90 \
    else if constexpr (N == 90) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90); \
    }

#define VISIT_CASE_91 \
    else if constexpr (N == 91) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91); \
    }

#define VISIT_CASE_92 \
    else if constexpr (N == 92) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92); \
    }

#define VISIT_CASE_93 \
    else if constexpr (N == 93) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93); \
    }

#define VISIT_CASE_94 \
    else if constexpr (N == 94) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94); \
    }

#define VISIT_CASE_95 \
    else if constexpr (N == 95) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95); \
    }

#define VISIT_CASE_96 \
    else if constexpr (N == 96) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96); \
    }

#define VISIT_CASE_97 \
    else if constexpr (N == 97) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97); \
    }

#define VISIT_CASE_98 \
    else if constexpr (N == 98) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98); \
    }

#define VISIT_CASE_99 \
    else if constexpr (N == 99) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99); \
    }

#define VISIT_CASE_100 \
    else if constexpr (N == 100) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100); \
    }

#define VISIT_CASE_101 \
    else if constexpr (N == 101) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101); \
    }

#define VISIT_CASE_102 \
    else if constexpr (N == 102) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102); \
    }

#define VISIT_CASE_103 \
    else if constexpr (N == 103) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103); \
    }

#define VISIT_CASE_104 \
    else if constexpr (N == 104) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104); \
    }

#define VISIT_CASE_105 \
    else if constexpr (N == 105) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105); \
    }

#define VISIT_CASE_106 \
    else if constexpr (N == 106) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106); \
    }

#define VISIT_CASE_107 \
    else if constexpr (N == 107) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107); \
    }

#define VISIT_CASE_108 \
    else if constexpr (N == 108) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108); \
    }

#define VISIT_CASE_109 \
    else if constexpr (N == 109) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109); \
    }

#define VISIT_CASE_110 \
    else if constexpr (N == 110) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110); \
    }

#define VISIT_CASE_111 \
    else if constexpr (N == 111) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111); \
    }

#define VISIT_CASE_112 \
    else if constexpr (N == 112) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112); \
    }

#define VISIT_CASE_113 \
    else if constexpr (N == 113) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113); \
    }

#define VISIT_CASE_114 \
    else if constexpr (N == 114) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114); \
    }

#define VISIT_CASE_115 \
    else if constexpr (N == 115) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115); \
    }

#define VISIT_CASE_116 \
    else if constexpr (N == 116) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116); \
    }

#define VISIT_CASE_117 \
    else if constexpr (N == 117) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117); \
    }

#define VISIT_CASE_118 \
    else if constexpr (N == 118) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118); \
    }

#define VISIT_CASE_119 \
    else if constexpr (N == 119) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119); \
    }

#define VISIT_CASE_120 \
    else if constexpr (N == 120) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120); \
    }

#define VISIT_CASE_121 \
    else if constexpr (N == 121) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121); \
    }

#define VISIT_CASE_122 \
    else if constexpr (N == 122) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122); \
    }

#define VISIT_CASE_123 \
    else if constexpr (N == 123) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123); \
    }

#define VISIT_CASE_124 \
    else if constexpr (N == 124) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124); \
    }

#define VISIT_CASE_125 \
    else if constexpr (N == 125) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125); \
    }

#define VISIT_CASE_126 \
    else if constexpr (N == 126) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126); \
    }

#define VISIT_CASE_127 \
    else if constexpr (N == 127) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126, a127] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126, a127); \
    }

#define VISIT_CASE_128 \
    else if constexpr (N == 128) \
    { \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126, a127, a128] = bevisited; \
        visit_helper(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126, a127, a128); \
    }

// 生成指定数量的分支
#define GENERATE_VISIT_CASES(MAX) VISIT_CASES_IMPL_##MAX

#define VISIT_CASES_IMPL_0 VISIT_CASE_0
#define VISIT_CASES_IMPL_1 VISIT_CASES_IMPL_0 VISIT_CASE_1
#define VISIT_CASES_IMPL_2 VISIT_CASES_IMPL_1 VISIT_CASE_2
#define VISIT_CASES_IMPL_3 VISIT_CASES_IMPL_2 VISIT_CASE_3
#define VISIT_CASES_IMPL_4 VISIT_CASES_IMPL_3 VISIT_CASE_4
#define VISIT_CASES_IMPL_5 VISIT_CASES_IMPL_4 VISIT_CASE_5
#define VISIT_CASES_IMPL_6 VISIT_CASES_IMPL_5 VISIT_CASE_6
#define VISIT_CASES_IMPL_7 VISIT_CASES_IMPL_6 VISIT_CASE_7
#define VISIT_CASES_IMPL_8 VISIT_CASES_IMPL_7 VISIT_CASE_8
#define VISIT_CASES_IMPL_9 VISIT_CASES_IMPL_8 VISIT_CASE_9
#define VISIT_CASES_IMPL_10 VISIT_CASES_IMPL_9 VISIT_CASE_10
#define VISIT_CASES_IMPL_11 VISIT_CASES_IMPL_10 VISIT_CASE_11
#define VISIT_CASES_IMPL_12 VISIT_CASES_IMPL_11 VISIT_CASE_12
#define VISIT_CASES_IMPL_13 VISIT_CASES_IMPL_12 VISIT_CASE_13
#define VISIT_CASES_IMPL_14 VISIT_CASES_IMPL_13 VISIT_CASE_14
#define VISIT_CASES_IMPL_15 VISIT_CASES_IMPL_14 VISIT_CASE_15
#define VISIT_CASES_IMPL_16 VISIT_CASES_IMPL_15 VISIT_CASE_16
#define VISIT_CASES_IMPL_17 VISIT_CASES_IMPL_16 VISIT_CASE_17
#define VISIT_CASES_IMPL_18 VISIT_CASES_IMPL_17 VISIT_CASE_18
#define VISIT_CASES_IMPL_19 VISIT_CASES_IMPL_18 VISIT_CASE_19
#define VISIT_CASES_IMPL_20 VISIT_CASES_IMPL_19 VISIT_CASE_20
#define VISIT_CASES_IMPL_21 VISIT_CASES_IMPL_20 VISIT_CASE_21
#define VISIT_CASES_IMPL_22 VISIT_CASES_IMPL_21 VISIT_CASE_22
#define VISIT_CASES_IMPL_23 VISIT_CASES_IMPL_22 VISIT_CASE_23
#define VISIT_CASES_IMPL_24 VISIT_CASES_IMPL_23 VISIT_CASE_24
#define VISIT_CASES_IMPL_25 VISIT_CASES_IMPL_24 VISIT_CASE_25
#define VISIT_CASES_IMPL_26 VISIT_CASES_IMPL_25 VISIT_CASE_26
#define VISIT_CASES_IMPL_27 VISIT_CASES_IMPL_26 VISIT_CASE_27
#define VISIT_CASES_IMPL_28 VISIT_CASES_IMPL_27 VISIT_CASE_28
#define VISIT_CASES_IMPL_29 VISIT_CASES_IMPL_28 VISIT_CASE_29
#define VISIT_CASES_IMPL_30 VISIT_CASES_IMPL_29 VISIT_CASE_30
#define VISIT_CASES_IMPL_31 VISIT_CASES_IMPL_30 VISIT_CASE_31
#define VISIT_CASES_IMPL_32 VISIT_CASES_IMPL_31 VISIT_CASE_32
#define VISIT_CASES_IMPL_33 VISIT_CASES_IMPL_32 VISIT_CASE_33
#define VISIT_CASES_IMPL_34 VISIT_CASES_IMPL_33 VISIT_CASE_34
#define VISIT_CASES_IMPL_35 VISIT_CASES_IMPL_34 VISIT_CASE_35
#define VISIT_CASES_IMPL_36 VISIT_CASES_IMPL_35 VISIT_CASE_36
#define VISIT_CASES_IMPL_37 VISIT_CASES_IMPL_36 VISIT_CASE_37
#define VISIT_CASES_IMPL_38 VISIT_CASES_IMPL_37 VISIT_CASE_38
#define VISIT_CASES_IMPL_39 VISIT_CASES_IMPL_38 VISIT_CASE_39
#define VISIT_CASES_IMPL_40 VISIT_CASES_IMPL_39 VISIT_CASE_40
#define VISIT_CASES_IMPL_41 VISIT_CASES_IMPL_40 VISIT_CASE_41
#define VISIT_CASES_IMPL_42 VISIT_CASES_IMPL_41 VISIT_CASE_42
#define VISIT_CASES_IMPL_43 VISIT_CASES_IMPL_42 VISIT_CASE_43
#define VISIT_CASES_IMPL_44 VISIT_CASES_IMPL_43 VISIT_CASE_44
#define VISIT_CASES_IMPL_45 VISIT_CASES_IMPL_44 VISIT_CASE_45
#define VISIT_CASES_IMPL_46 VISIT_CASES_IMPL_45 VISIT_CASE_46
#define VISIT_CASES_IMPL_47 VISIT_CASES_IMPL_46 VISIT_CASE_47
#define VISIT_CASES_IMPL_48 VISIT_CASES_IMPL_47 VISIT_CASE_48
#define VISIT_CASES_IMPL_49 VISIT_CASES_IMPL_48 VISIT_CASE_49
#define VISIT_CASES_IMPL_50 VISIT_CASES_IMPL_49 VISIT_CASE_50
#define VISIT_CASES_IMPL_51 VISIT_CASES_IMPL_50 VISIT_CASE_51
#define VISIT_CASES_IMPL_52 VISIT_CASES_IMPL_51 VISIT_CASE_52
#define VISIT_CASES_IMPL_53 VISIT_CASES_IMPL_52 VISIT_CASE_53
#define VISIT_CASES_IMPL_54 VISIT_CASES_IMPL_53 VISIT_CASE_54
#define VISIT_CASES_IMPL_55 VISIT_CASES_IMPL_54 VISIT_CASE_55
#define VISIT_CASES_IMPL_56 VISIT_CASES_IMPL_55 VISIT_CASE_56
#define VISIT_CASES_IMPL_57 VISIT_CASES_IMPL_56 VISIT_CASE_57
#define VISIT_CASES_IMPL_58 VISIT_CASES_IMPL_57 VISIT_CASE_58
#define VISIT_CASES_IMPL_59 VISIT_CASES_IMPL_58 VISIT_CASE_59
#define VISIT_CASES_IMPL_60 VISIT_CASES_IMPL_59 VISIT_CASE_60
#define VISIT_CASES_IMPL_61 VISIT_CASES_IMPL_60 VISIT_CASE_61
#define VISIT_CASES_IMPL_62 VISIT_CASES_IMPL_61 VISIT_CASE_62
#define VISIT_CASES_IMPL_63 VISIT_CASES_IMPL_62 VISIT_CASE_63
#define VISIT_CASES_IMPL_64 VISIT_CASES_IMPL_63 VISIT_CASE_64
#define VISIT_CASES_IMPL_65 VISIT_CASES_IMPL_64 VISIT_CASE_65
#define VISIT_CASES_IMPL_66 VISIT_CASES_IMPL_65 VISIT_CASE_66
#define VISIT_CASES_IMPL_67 VISIT_CASES_IMPL_66 VISIT_CASE_67
#define VISIT_CASES_IMPL_68 VISIT_CASES_IMPL_67 VISIT_CASE_68
#define VISIT_CASES_IMPL_69 VISIT_CASES_IMPL_68 VISIT_CASE_69
#define VISIT_CASES_IMPL_70 VISIT_CASES_IMPL_69 VISIT_CASE_70
#define VISIT_CASES_IMPL_71 VISIT_CASES_IMPL_70 VISIT_CASE_71
#define VISIT_CASES_IMPL_72 VISIT_CASES_IMPL_71 VISIT_CASE_72
#define VISIT_CASES_IMPL_73 VISIT_CASES_IMPL_72 VISIT_CASE_73
#define VISIT_CASES_IMPL_74 VISIT_CASES_IMPL_73 VISIT_CASE_74
#define VISIT_CASES_IMPL_75 VISIT_CASES_IMPL_74 VISIT_CASE_75
#define VISIT_CASES_IMPL_76 VISIT_CASES_IMPL_75 VISIT_CASE_76
#define VISIT_CASES_IMPL_77 VISIT_CASES_IMPL_76 VISIT_CASE_77
#define VISIT_CASES_IMPL_78 VISIT_CASES_IMPL_77 VISIT_CASE_78
#define VISIT_CASES_IMPL_79 VISIT_CASES_IMPL_78 VISIT_CASE_79
#define VISIT_CASES_IMPL_80 VISIT_CASES_IMPL_79 VISIT_CASE_80
#define VISIT_CASES_IMPL_81 VISIT_CASES_IMPL_80 VISIT_CASE_81
#define VISIT_CASES_IMPL_82 VISIT_CASES_IMPL_81 VISIT_CASE_82
#define VISIT_CASES_IMPL_83 VISIT_CASES_IMPL_82 VISIT_CASE_83
#define VISIT_CASES_IMPL_84 VISIT_CASES_IMPL_83 VISIT_CASE_84
#define VISIT_CASES_IMPL_85 VISIT_CASES_IMPL_84 VISIT_CASE_85
#define VISIT_CASES_IMPL_86 VISIT_CASES_IMPL_85 VISIT_CASE_86
#define VISIT_CASES_IMPL_87 VISIT_CASES_IMPL_86 VISIT_CASE_87
#define VISIT_CASES_IMPL_88 VISIT_CASES_IMPL_87 VISIT_CASE_88
#define VISIT_CASES_IMPL_89 VISIT_CASES_IMPL_88 VISIT_CASE_89
#define VISIT_CASES_IMPL_90 VISIT_CASES_IMPL_89 VISIT_CASE_90
#define VISIT_CASES_IMPL_91 VISIT_CASES_IMPL_90 VISIT_CASE_91
#define VISIT_CASES_IMPL_92 VISIT_CASES_IMPL_91 VISIT_CASE_92
#define VISIT_CASES_IMPL_93 VISIT_CASES_IMPL_92 VISIT_CASE_93
#define VISIT_CASES_IMPL_94 VISIT_CASES_IMPL_93 VISIT_CASE_94
#define VISIT_CASES_IMPL_95 VISIT_CASES_IMPL_94 VISIT_CASE_95
#define VISIT_CASES_IMPL_96 VISIT_CASES_IMPL_95 VISIT_CASE_96
#define VISIT_CASES_IMPL_97 VISIT_CASES_IMPL_96 VISIT_CASE_97
#define VISIT_CASES_IMPL_98 VISIT_CASES_IMPL_97 VISIT_CASE_98
#define VISIT_CASES_IMPL_99 VISIT_CASES_IMPL_98 VISIT_CASE_99
#define VISIT_CASES_IMPL_100 VISIT_CASES_IMPL_99 VISIT_CASE_100
#define VISIT_CASES_IMPL_101 VISIT_CASES_IMPL_100 VISIT_CASE_101
#define VISIT_CASES_IMPL_102 VISIT_CASES_IMPL_101 VISIT_CASE_102
#define VISIT_CASES_IMPL_103 VISIT_CASES_IMPL_102 VISIT_CASE_103
#define VISIT_CASES_IMPL_104 VISIT_CASES_IMPL_103 VISIT_CASE_104
#define VISIT_CASES_IMPL_105 VISIT_CASES_IMPL_104 VISIT_CASE_105
#define VISIT_CASES_IMPL_106 VISIT_CASES_IMPL_105 VISIT_CASE_106
#define VISIT_CASES_IMPL_107 VISIT_CASES_IMPL_106 VISIT_CASE_107
#define VISIT_CASES_IMPL_108 VISIT_CASES_IMPL_107 VISIT_CASE_108
#define VISIT_CASES_IMPL_109 VISIT_CASES_IMPL_108 VISIT_CASE_109
#define VISIT_CASES_IMPL_110 VISIT_CASES_IMPL_109 VISIT_CASE_110
#define VISIT_CASES_IMPL_111 VISIT_CASES_IMPL_110 VISIT_CASE_111
#define VISIT_CASES_IMPL_112 VISIT_CASES_IMPL_111 VISIT_CASE_112
#define VISIT_CASES_IMPL_113 VISIT_CASES_IMPL_112 VISIT_CASE_113
#define VISIT_CASES_IMPL_114 VISIT_CASES_IMPL_113 VISIT_CASE_114
#define VISIT_CASES_IMPL_115 VISIT_CASES_IMPL_114 VISIT_CASE_115
#define VISIT_CASES_IMPL_116 VISIT_CASES_IMPL_115 VISIT_CASE_116
#define VISIT_CASES_IMPL_117 VISIT_CASES_IMPL_116 VISIT_CASE_117
#define VISIT_CASES_IMPL_118 VISIT_CASES_IMPL_117 VISIT_CASE_118
#define VISIT_CASES_IMPL_119 VISIT_CASES_IMPL_118 VISIT_CASE_119
#define VISIT_CASES_IMPL_120 VISIT_CASES_IMPL_119 VISIT_CASE_120
#define VISIT_CASES_IMPL_121 VISIT_CASES_IMPL_120 VISIT_CASE_121
#define VISIT_CASES_IMPL_122 VISIT_CASES_IMPL_121 VISIT_CASE_122
#define VISIT_CASES_IMPL_123 VISIT_CASES_IMPL_122 VISIT_CASE_123
#define VISIT_CASES_IMPL_124 VISIT_CASES_IMPL_123 VISIT_CASE_124
#define VISIT_CASES_IMPL_125 VISIT_CASES_IMPL_124 VISIT_CASE_125
#define VISIT_CASES_IMPL_126 VISIT_CASES_IMPL_125 VISIT_CASE_126
#define VISIT_CASES_IMPL_127 VISIT_CASES_IMPL_126 VISIT_CASE_127
#define VISIT_CASES_IMPL_128 VISIT_CASES_IMPL_127 VISIT_CASE_128

// 为make_static_memberptr_tuple_form_type添加的宏
#define TUPLE_CASE_0 \
    if constexpr (Count == 0) \
    { \
        return std::tuple<>{}; \
    }

#define TUPLE_CASE_1 \
    else if constexpr (Count == 1) \
    { \
        static constexpr T obj{}; \
        auto&& [a1] = obj; \
        return std::make_tuple(&a1); \
    }

#define TUPLE_CASE_2 \
    else if constexpr (Count == 2) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2] = obj; \
        return std::make_tuple(&a1, &a2); \
    }

#define TUPLE_CASE_3 \
    else if constexpr (Count == 3) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3] = obj; \
        return std::make_tuple(&a1, &a2, &a3); \
    }

#define TUPLE_CASE_4 \
    else if constexpr (Count == 4) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4); \
    }

#define TUPLE_CASE_5 \
    else if constexpr (Count == 5) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5); \
    }

#define TUPLE_CASE_6 \
    else if constexpr (Count == 6) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6); \
    }

#define TUPLE_CASE_7 \
    else if constexpr (Count == 7) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7); \
    }

#define TUPLE_CASE_8 \
    else if constexpr (Count == 8) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8); \
    }

#define TUPLE_CASE_9 \
    else if constexpr (Count == 9) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9); \
    }

#define TUPLE_CASE_10 \
    else if constexpr (Count == 10) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10); \
    }

#define TUPLE_CASE_11 \
    else if constexpr (Count == 11) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11); \
    }

#define TUPLE_CASE_12 \
    else if constexpr (Count == 12) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12); \
    }

#define TUPLE_CASE_13 \
    else if constexpr (Count == 13) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13); \
    }

#define TUPLE_CASE_14 \
    else if constexpr (Count == 14) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14); \
    }

#define TUPLE_CASE_15 \
    else if constexpr (Count == 15) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15); \
    }

#define TUPLE_CASE_16 \
    else if constexpr (Count == 16) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16); \
    }

#define TUPLE_CASE_17 \
    else if constexpr (Count == 17) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17); \
    }

#define TUPLE_CASE_18 \
    else if constexpr (Count == 18) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18); \
    }

#define TUPLE_CASE_19 \
    else if constexpr (Count == 19) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19); \
    }

#define TUPLE_CASE_20 \
    else if constexpr (Count == 20) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20); \
    }

#define TUPLE_CASE_21 \
    else if constexpr (Count == 21) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21); \
    }

#define TUPLE_CASE_22 \
    else if constexpr (Count == 22) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22); \
    }

#define TUPLE_CASE_23 \
    else if constexpr (Count == 23) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23); \
    }

#define TUPLE_CASE_24 \
    else if constexpr (Count == 24) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24); \
    }

#define TUPLE_CASE_25 \
    else if constexpr (Count == 25) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25); \
    }

#define TUPLE_CASE_26 \
    else if constexpr (Count == 26) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26); \
    }

#define TUPLE_CASE_27 \
    else if constexpr (Count == 27) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27); \
    }

#define TUPLE_CASE_28 \
    else if constexpr (Count == 28) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28); \
    }

#define TUPLE_CASE_29 \
    else if constexpr (Count == 29) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29); \
    }

#define TUPLE_CASE_30 \
    else if constexpr (Count == 30) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30); \
    }

#define TUPLE_CASE_31 \
    else if constexpr (Count == 31) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31); \
    }

#define TUPLE_CASE_32 \
    else if constexpr (Count == 32) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32); \
    }

#define TUPLE_CASE_33 \
    else if constexpr (Count == 33) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33); \
    }

#define TUPLE_CASE_34 \
    else if constexpr (Count == 34) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34); \
    }

#define TUPLE_CASE_35 \
    else if constexpr (Count == 35) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35); \
    }

#define TUPLE_CASE_36 \
    else if constexpr (Count == 36) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36); \
    }

#define TUPLE_CASE_37 \
    else if constexpr (Count == 37) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37); \
    }

#define TUPLE_CASE_38 \
    else if constexpr (Count == 38) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38); \
    }

#define TUPLE_CASE_39 \
    else if constexpr (Count == 39) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39); \
    }

#define TUPLE_CASE_40 \
    else if constexpr (Count == 40) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40); \
    }

#define TUPLE_CASE_41 \
    else if constexpr (Count == 41) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41); \
    }

#define TUPLE_CASE_42 \
    else if constexpr (Count == 42) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42); \
    }

#define TUPLE_CASE_43 \
    else if constexpr (Count == 43) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43); \
    }

#define TUPLE_CASE_44 \
    else if constexpr (Count == 44) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44); \
    }

#define TUPLE_CASE_45 \
    else if constexpr (Count == 45) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45); \
    }

#define TUPLE_CASE_46 \
    else if constexpr (Count == 46) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46); \
    }

#define TUPLE_CASE_47 \
    else if constexpr (Count == 47) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47); \
    }

#define TUPLE_CASE_48 \
    else if constexpr (Count == 48) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48); \
    }

#define TUPLE_CASE_49 \
    else if constexpr (Count == 49) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49); \
    }

#define TUPLE_CASE_50 \
    else if constexpr (Count == 50) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50); \
    }

#define TUPLE_CASE_51 \
    else if constexpr (Count == 51) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51); \
    }

#define TUPLE_CASE_52 \
    else if constexpr (Count == 52) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52); \
    }

#define TUPLE_CASE_53 \
    else if constexpr (Count == 53) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53); \
    }

#define TUPLE_CASE_54 \
    else if constexpr (Count == 54) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54); \
    }

#define TUPLE_CASE_55 \
    else if constexpr (Count == 55) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55); \
    }

#define TUPLE_CASE_56 \
    else if constexpr (Count == 56) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56); \
    }

#define TUPLE_CASE_57 \
    else if constexpr (Count == 57) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57); \
    }

#define TUPLE_CASE_58 \
    else if constexpr (Count == 58) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58); \
    }

#define TUPLE_CASE_59 \
    else if constexpr (Count == 59) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59); \
    }

#define TUPLE_CASE_60 \
    else if constexpr (Count == 60) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60); \
    }

#define TUPLE_CASE_61 \
    else if constexpr (Count == 61) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61); \
    }

#define TUPLE_CASE_62 \
    else if constexpr (Count == 62) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62); \
    }

#define TUPLE_CASE_63 \
    else if constexpr (Count == 63) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63); \
    }

#define TUPLE_CASE_64 \
    else if constexpr (Count == 64) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64); \
    }

#define TUPLE_CASE_65 \
    else if constexpr (Count == 65) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65); \
    }

#define TUPLE_CASE_66 \
    else if constexpr (Count == 66) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66); \
    }

#define TUPLE_CASE_67 \
    else if constexpr (Count == 67) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67); \
    }

#define TUPLE_CASE_68 \
    else if constexpr (Count == 68) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68); \
    }

#define TUPLE_CASE_69 \
    else if constexpr (Count == 69) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69); \
    }

#define TUPLE_CASE_70 \
    else if constexpr (Count == 70) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70); \
    }

#define TUPLE_CASE_71 \
    else if constexpr (Count == 71) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71); \
    }

#define TUPLE_CASE_72 \
    else if constexpr (Count == 72) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72); \
    }

#define TUPLE_CASE_73 \
    else if constexpr (Count == 73) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73); \
    }

#define TUPLE_CASE_74 \
    else if constexpr (Count == 74) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74); \
    }

#define TUPLE_CASE_75 \
    else if constexpr (Count == 75) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75); \
    }

#define TUPLE_CASE_76 \
    else if constexpr (Count == 76) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76); \
    }

#define TUPLE_CASE_77 \
    else if constexpr (Count == 77) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77); \
    }

#define TUPLE_CASE_78 \
    else if constexpr (Count == 78) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78); \
    }

#define TUPLE_CASE_79 \
    else if constexpr (Count == 79) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79); \
    }

#define TUPLE_CASE_80 \
    else if constexpr (Count == 80) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80); \
    }

#define TUPLE_CASE_81 \
    else if constexpr (Count == 81) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81); \
    }

#define TUPLE_CASE_82 \
    else if constexpr (Count == 82) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82); \
    }

#define TUPLE_CASE_83 \
    else if constexpr (Count == 83) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83); \
    }

#define TUPLE_CASE_84 \
    else if constexpr (Count == 84) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84); \
    }

#define TUPLE_CASE_85 \
    else if constexpr (Count == 85) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85); \
    }

#define TUPLE_CASE_86 \
    else if constexpr (Count == 86) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86); \
    }

#define TUPLE_CASE_87 \
    else if constexpr (Count == 87) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87); \
    }

#define TUPLE_CASE_88 \
    else if constexpr (Count == 88) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88); \
    }

#define TUPLE_CASE_89 \
    else if constexpr (Count == 89) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89); \
    }

#define TUPLE_CASE_90 \
    else if constexpr (Count == 90) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90); \
    }

#define TUPLE_CASE_91 \
    else if constexpr (Count == 91) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91); \
    }

#define TUPLE_CASE_92 \
    else if constexpr (Count == 92) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92); \
    }

#define TUPLE_CASE_93 \
    else if constexpr (Count == 93) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93); \
    }

#define TUPLE_CASE_94 \
    else if constexpr (Count == 94) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94); \
    }

#define TUPLE_CASE_95 \
    else if constexpr (Count == 95) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95); \
    }

#define TUPLE_CASE_96 \
    else if constexpr (Count == 96) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96); \
    }

#define TUPLE_CASE_97 \
    else if constexpr (Count == 97) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97); \
    }

#define TUPLE_CASE_98 \
    else if constexpr (Count == 98) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98); \
    }

#define TUPLE_CASE_99 \
    else if constexpr (Count == 99) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99); \
    }

#define TUPLE_CASE_100 \
    else if constexpr (Count == 100) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100); \
    }

#define TUPLE_CASE_101 \
    else if constexpr (Count == 101) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101); \
    }

#define TUPLE_CASE_102 \
    else if constexpr (Count == 102) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102); \
    }

#define TUPLE_CASE_103 \
    else if constexpr (Count == 103) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103); \
    }

#define TUPLE_CASE_104 \
    else if constexpr (Count == 104) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104); \
    }

#define TUPLE_CASE_105 \
    else if constexpr (Count == 105) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105); \
    }

#define TUPLE_CASE_106 \
    else if constexpr (Count == 106) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106); \
    }

#define TUPLE_CASE_107 \
    else if constexpr (Count == 107) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107); \
    }

#define TUPLE_CASE_108 \
    else if constexpr (Count == 108) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108); \
    }

#define TUPLE_CASE_109 \
    else if constexpr (Count == 109) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109); \
    }

#define TUPLE_CASE_110 \
    else if constexpr (Count == 110) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110); \
    }

#define TUPLE_CASE_111 \
    else if constexpr (Count == 111) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111); \
    }

#define TUPLE_CASE_112 \
    else if constexpr (Count == 112) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112); \
    }

#define TUPLE_CASE_113 \
    else if constexpr (Count == 113) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113); \
    }

#define TUPLE_CASE_114 \
    else if constexpr (Count == 114) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114); \
    }

#define TUPLE_CASE_115 \
    else if constexpr (Count == 115) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115); \
    }

#define TUPLE_CASE_116 \
    else if constexpr (Count == 116) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116); \
    }

#define TUPLE_CASE_117 \
    else if constexpr (Count == 117) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117); \
    }

#define TUPLE_CASE_118 \
    else if constexpr (Count == 118) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118); \
    }

#define TUPLE_CASE_119 \
    else if constexpr (Count == 119) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119); \
    }

#define TUPLE_CASE_120 \
    else if constexpr (Count == 120) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120); \
    }

#define TUPLE_CASE_121 \
    else if constexpr (Count == 121) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120, &a121); \
    }

#define TUPLE_CASE_122 \
    else if constexpr (Count == 122) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120, &a121, &a122); \
    }

#define TUPLE_CASE_123 \
    else if constexpr (Count == 123) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120, &a121, &a122, &a123); \
    }

#define TUPLE_CASE_124 \
    else if constexpr (Count == 124) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120, &a121, &a122, &a123, &a124); \
    }

#define TUPLE_CASE_125 \
    else if constexpr (Count == 125) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120, &a121, &a122, &a123, &a124, &a125); \
    }

#define TUPLE_CASE_126 \
    else if constexpr (Count == 126) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120, &a121, &a122, &a123, &a124, &a125, &a126); \
    }

#define TUPLE_CASE_127 \
    else if constexpr (Count == 127) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126, a127] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120, &a121, &a122, &a123, &a124, &a125, &a126, &a127); \
    }

#define TUPLE_CASE_128 \
    else if constexpr (Count == 128) \
    { \
        static constexpr T obj{}; \
        auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, a120, a121, a122, a123, a124, a125, a126, a127, a128] = obj; \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70, &a71, &a72, &a73, &a74, &a75, &a76, &a77, &a78, &a79, &a80, &a81, &a82, &a83, &a84, &a85, &a86, &a87, &a88, &a89, &a90, &a91, &a92, &a93, &a94, &a95, &a96, &a97, &a98, &a99, &a100, &a101, &a102, &a103, &a104, &a105, &a106, &a107, &a108, &a109, &a110, &a111, &a112, &a113, &a114, &a115, &a116, &a117, &a118, &a119, &a120, &a121, &a122, &a123, &a124, &a125, &a126, &a127, &a128); \
    }

// 生成指定数量的tuple分支
#define GENERATE_TUPLE_CASES(MAX) TUPLE_CASES_IMPL_##MAX

#define TUPLE_CASES_IMPL_0 TUPLE_CASE_0
#define TUPLE_CASES_IMPL_1 TUPLE_CASES_IMPL_0 TUPLE_CASE_1
#define TUPLE_CASES_IMPL_2 TUPLE_CASES_IMPL_1 TUPLE_CASE_2
#define TUPLE_CASES_IMPL_3 TUPLE_CASES_IMPL_2 TUPLE_CASE_3
#define TUPLE_CASES_IMPL_4 TUPLE_CASES_IMPL_3 TUPLE_CASE_4
#define TUPLE_CASES_IMPL_5 TUPLE_CASES_IMPL_4 TUPLE_CASE_5
#define TUPLE_CASES_IMPL_6 TUPLE_CASES_IMPL_5 TUPLE_CASE_6
#define TUPLE_CASES_IMPL_7 TUPLE_CASES_IMPL_6 TUPLE_CASE_7
#define TUPLE_CASES_IMPL_8 TUPLE_CASES_IMPL_7 TUPLE_CASE_8
#define TUPLE_CASES_IMPL_9 TUPLE_CASES_IMPL_8 TUPLE_CASE_9
#define TUPLE_CASES_IMPL_10 TUPLE_CASES_IMPL_9 TUPLE_CASE_10
#define TUPLE_CASES_IMPL_11 TUPLE_CASES_IMPL_10 TUPLE_CASE_11
#define TUPLE_CASES_IMPL_12 TUPLE_CASES_IMPL_11 TUPLE_CASE_12
#define TUPLE_CASES_IMPL_13 TUPLE_CASES_IMPL_12 TUPLE_CASE_13
#define TUPLE_CASES_IMPL_14 TUPLE_CASES_IMPL_13 TUPLE_CASE_14
#define TUPLE_CASES_IMPL_15 TUPLE_CASES_IMPL_14 TUPLE_CASE_15
#define TUPLE_CASES_IMPL_16 TUPLE_CASES_IMPL_15 TUPLE_CASE_16
#define TUPLE_CASES_IMPL_17 TUPLE_CASES_IMPL_16 TUPLE_CASE_17
#define TUPLE_CASES_IMPL_18 TUPLE_CASES_IMPL_17 TUPLE_CASE_18
#define TUPLE_CASES_IMPL_19 TUPLE_CASES_IMPL_18 TUPLE_CASE_19
#define TUPLE_CASES_IMPL_20 TUPLE_CASES_IMPL_19 TUPLE_CASE_20
#define TUPLE_CASES_IMPL_21 TUPLE_CASES_IMPL_20 TUPLE_CASE_21
#define TUPLE_CASES_IMPL_22 TUPLE_CASES_IMPL_21 TUPLE_CASE_22
#define TUPLE_CASES_IMPL_23 TUPLE_CASES_IMPL_22 TUPLE_CASE_23
#define TUPLE_CASES_IMPL_24 TUPLE_CASES_IMPL_23 TUPLE_CASE_24
#define TUPLE_CASES_IMPL_25 TUPLE_CASES_IMPL_24 TUPLE_CASE_25
#define TUPLE_CASES_IMPL_26 TUPLE_CASES_IMPL_25 TUPLE_CASE_26
#define TUPLE_CASES_IMPL_27 TUPLE_CASES_IMPL_26 TUPLE_CASE_27
#define TUPLE_CASES_IMPL_28 TUPLE_CASES_IMPL_27 TUPLE_CASE_28
#define TUPLE_CASES_IMPL_29 TUPLE_CASES_IMPL_28 TUPLE_CASE_29
#define TUPLE_CASES_IMPL_30 TUPLE_CASES_IMPL_29 TUPLE_CASE_30
#define TUPLE_CASES_IMPL_31 TUPLE_CASES_IMPL_30 TUPLE_CASE_31
#define TUPLE_CASES_IMPL_32 TUPLE_CASES_IMPL_31 TUPLE_CASE_32
#define TUPLE_CASES_IMPL_33 TUPLE_CASES_IMPL_32 TUPLE_CASE_33
#define TUPLE_CASES_IMPL_34 TUPLE_CASES_IMPL_33 TUPLE_CASE_34
#define TUPLE_CASES_IMPL_35 TUPLE_CASES_IMPL_34 TUPLE_CASE_35
#define TUPLE_CASES_IMPL_36 TUPLE_CASES_IMPL_35 TUPLE_CASE_36
#define TUPLE_CASES_IMPL_37 TUPLE_CASES_IMPL_36 TUPLE_CASE_37
#define TUPLE_CASES_IMPL_38 TUPLE_CASES_IMPL_37 TUPLE_CASE_38
#define TUPLE_CASES_IMPL_39 TUPLE_CASES_IMPL_38 TUPLE_CASE_39
#define TUPLE_CASES_IMPL_40 TUPLE_CASES_IMPL_39 TUPLE_CASE_40
#define TUPLE_CASES_IMPL_41 TUPLE_CASES_IMPL_40 TUPLE_CASE_41
#define TUPLE_CASES_IMPL_42 TUPLE_CASES_IMPL_41 TUPLE_CASE_42
#define TUPLE_CASES_IMPL_43 TUPLE_CASES_IMPL_42 TUPLE_CASE_43
#define TUPLE_CASES_IMPL_44 TUPLE_CASES_IMPL_43 TUPLE_CASE_44
#define TUPLE_CASES_IMPL_45 TUPLE_CASES_IMPL_44 TUPLE_CASE_45
#define TUPLE_CASES_IMPL_46 TUPLE_CASES_IMPL_45 TUPLE_CASE_46
#define TUPLE_CASES_IMPL_47 TUPLE_CASES_IMPL_46 TUPLE_CASE_47
#define TUPLE_CASES_IMPL_48 TUPLE_CASES_IMPL_47 TUPLE_CASE_48
#define TUPLE_CASES_IMPL_49 TUPLE_CASES_IMPL_48 TUPLE_CASE_49
#define TUPLE_CASES_IMPL_50 TUPLE_CASES_IMPL_49 TUPLE_CASE_50
#define TUPLE_CASES_IMPL_51 TUPLE_CASES_IMPL_50 TUPLE_CASE_51
#define TUPLE_CASES_IMPL_52 TUPLE_CASES_IMPL_51 TUPLE_CASE_52
#define TUPLE_CASES_IMPL_53 TUPLE_CASES_IMPL_52 TUPLE_CASE_53
#define TUPLE_CASES_IMPL_54 TUPLE_CASES_IMPL_53 TUPLE_CASE_54
#define TUPLE_CASES_IMPL_55 TUPLE_CASES_IMPL_54 TUPLE_CASE_55
#define TUPLE_CASES_IMPL_56 TUPLE_CASES_IMPL_55 TUPLE_CASE_56
#define TUPLE_CASES_IMPL_57 TUPLE_CASES_IMPL_56 TUPLE_CASE_57
#define TUPLE_CASES_IMPL_58 TUPLE_CASES_IMPL_57 TUPLE_CASE_58
#define TUPLE_CASES_IMPL_59 TUPLE_CASES_IMPL_58 TUPLE_CASE_59
#define TUPLE_CASES_IMPL_60 TUPLE_CASES_IMPL_59 TUPLE_CASE_60
#define TUPLE_CASES_IMPL_61 TUPLE_CASES_IMPL_60 TUPLE_CASE_61
#define TUPLE_CASES_IMPL_62 TUPLE_CASES_IMPL_61 TUPLE_CASE_62
#define TUPLE_CASES_IMPL_63 TUPLE_CASES_IMPL_62 TUPLE_CASE_63
#define TUPLE_CASES_IMPL_64 TUPLE_CASES_IMPL_63 TUPLE_CASE_64
#define TUPLE_CASES_IMPL_65 TUPLE_CASES_IMPL_64 TUPLE_CASE_65
#define TUPLE_CASES_IMPL_66 TUPLE_CASES_IMPL_65 TUPLE_CASE_66
#define TUPLE_CASES_IMPL_67 TUPLE_CASES_IMPL_66 TUPLE_CASE_67
#define TUPLE_CASES_IMPL_68 TUPLE_CASES_IMPL_67 TUPLE_CASE_68
#define TUPLE_CASES_IMPL_69 TUPLE_CASES_IMPL_68 TUPLE_CASE_69
#define TUPLE_CASES_IMPL_70 TUPLE_CASES_IMPL_69 TUPLE_CASE_70
#define TUPLE_CASES_IMPL_71 TUPLE_CASES_IMPL_70 TUPLE_CASE_71
#define TUPLE_CASES_IMPL_72 TUPLE_CASES_IMPL_71 TUPLE_CASE_72
#define TUPLE_CASES_IMPL_73 TUPLE_CASES_IMPL_72 TUPLE_CASE_73
#define TUPLE_CASES_IMPL_74 TUPLE_CASES_IMPL_73 TUPLE_CASE_74
#define TUPLE_CASES_IMPL_75 TUPLE_CASES_IMPL_74 TUPLE_CASE_75
#define TUPLE_CASES_IMPL_76 TUPLE_CASES_IMPL_75 TUPLE_CASE_76
#define TUPLE_CASES_IMPL_77 TUPLE_CASES_IMPL_76 TUPLE_CASE_77
#define TUPLE_CASES_IMPL_78 TUPLE_CASES_IMPL_77 TUPLE_CASE_78
#define TUPLE_CASES_IMPL_79 TUPLE_CASES_IMPL_78 TUPLE_CASE_79
#define TUPLE_CASES_IMPL_80 TUPLE_CASES_IMPL_79 TUPLE_CASE_80
#define TUPLE_CASES_IMPL_81 TUPLE_CASES_IMPL_80 TUPLE_CASE_81
#define TUPLE_CASES_IMPL_82 TUPLE_CASES_IMPL_81 TUPLE_CASE_82
#define TUPLE_CASES_IMPL_83 TUPLE_CASES_IMPL_82 TUPLE_CASE_83
#define TUPLE_CASES_IMPL_84 TUPLE_CASES_IMPL_83 TUPLE_CASE_84
#define TUPLE_CASES_IMPL_85 TUPLE_CASES_IMPL_84 TUPLE_CASE_85
#define TUPLE_CASES_IMPL_86 TUPLE_CASES_IMPL_85 TUPLE_CASE_86
#define TUPLE_CASES_IMPL_87 TUPLE_CASES_IMPL_86 TUPLE_CASE_87
#define TUPLE_CASES_IMPL_88 TUPLE_CASES_IMPL_87 TUPLE_CASE_88
#define TUPLE_CASES_IMPL_89 TUPLE_CASES_IMPL_88 TUPLE_CASE_89
#define TUPLE_CASES_IMPL_90 TUPLE_CASES_IMPL_89 TUPLE_CASE_90
#define TUPLE_CASES_IMPL_91 TUPLE_CASES_IMPL_90 TUPLE_CASE_91
#define TUPLE_CASES_IMPL_92 TUPLE_CASES_IMPL_91 TUPLE_CASE_92
#define TUPLE_CASES_IMPL_93 TUPLE_CASES_IMPL_92 TUPLE_CASE_93
#define TUPLE_CASES_IMPL_94 TUPLE_CASES_IMPL_93 TUPLE_CASE_94
#define TUPLE_CASES_IMPL_95 TUPLE_CASES_IMPL_94 TUPLE_CASE_95
#define TUPLE_CASES_IMPL_96 TUPLE_CASES_IMPL_95 TUPLE_CASE_96
#define TUPLE_CASES_IMPL_97 TUPLE_CASES_IMPL_96 TUPLE_CASE_97
#define TUPLE_CASES_IMPL_98 TUPLE_CASES_IMPL_97 TUPLE_CASE_98
#define TUPLE_CASES_IMPL_99 TUPLE_CASES_IMPL_98 TUPLE_CASE_99
#define TUPLE_CASES_IMPL_100 TUPLE_CASES_IMPL_99 TUPLE_CASE_100
#define TUPLE_CASES_IMPL_101 TUPLE_CASES_IMPL_100 TUPLE_CASE_101
#define TUPLE_CASES_IMPL_102 TUPLE_CASES_IMPL_101 TUPLE_CASE_102
#define TUPLE_CASES_IMPL_103 TUPLE_CASES_IMPL_102 TUPLE_CASE_103
#define TUPLE_CASES_IMPL_104 TUPLE_CASES_IMPL_103 TUPLE_CASE_104
#define TUPLE_CASES_IMPL_105 TUPLE_CASES_IMPL_104 TUPLE_CASE_105
#define TUPLE_CASES_IMPL_106 TUPLE_CASES_IMPL_105 TUPLE_CASE_106
#define TUPLE_CASES_IMPL_107 TUPLE_CASES_IMPL_106 TUPLE_CASE_107
#define TUPLE_CASES_IMPL_108 TUPLE_CASES_IMPL_107 TUPLE_CASE_108
#define TUPLE_CASES_IMPL_109 TUPLE_CASES_IMPL_108 TUPLE_CASE_109
#define TUPLE_CASES_IMPL_110 TUPLE_CASES_IMPL_109 TUPLE_CASE_110
#define TUPLE_CASES_IMPL_111 TUPLE_CASES_IMPL_110 TUPLE_CASE_111
#define TUPLE_CASES_IMPL_112 TUPLE_CASES_IMPL_111 TUPLE_CASE_112
#define TUPLE_CASES_IMPL_113 TUPLE_CASES_IMPL_112 TUPLE_CASE_113
#define TUPLE_CASES_IMPL_114 TUPLE_CASES_IMPL_113 TUPLE_CASE_114
#define TUPLE_CASES_IMPL_115 TUPLE_CASES_IMPL_114 TUPLE_CASE_115
#define TUPLE_CASES_IMPL_116 TUPLE_CASES_IMPL_115 TUPLE_CASE_116
#define TUPLE_CASES_IMPL_117 TUPLE_CASES_IMPL_116 TUPLE_CASE_117
#define TUPLE_CASES_IMPL_118 TUPLE_CASES_IMPL_117 TUPLE_CASE_118
#define TUPLE_CASES_IMPL_119 TUPLE_CASES_IMPL_118 TUPLE_CASE_119
#define TUPLE_CASES_IMPL_120 TUPLE_CASES_IMPL_119 TUPLE_CASE_120
#define TUPLE_CASES_IMPL_121 TUPLE_CASES_IMPL_120 TUPLE_CASE_121
#define TUPLE_CASES_IMPL_122 TUPLE_CASES_IMPL_121 TUPLE_CASE_122
#define TUPLE_CASES_IMPL_123 TUPLE_CASES_IMPL_122 TUPLE_CASE_123
#define TUPLE_CASES_IMPL_124 TUPLE_CASES_IMPL_123 TUPLE_CASE_124
#define TUPLE_CASES_IMPL_125 TUPLE_CASES_IMPL_124 TUPLE_CASE_125
#define TUPLE_CASES_IMPL_126 TUPLE_CASES_IMPL_125 TUPLE_CASE_126
#define TUPLE_CASES_IMPL_127 TUPLE_CASES_IMPL_126 TUPLE_CASE_127
#define TUPLE_CASES_IMPL_128 TUPLE_CASES_IMPL_127 TUPLE_CASE_128

#endif // REFLECTION_MACROS_HPP
