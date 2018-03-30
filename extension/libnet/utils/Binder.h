//--------------------------------------------------------------------
// 文件名:		Binder.h
// 内  容:		C++11 binder macro
// 说  明:		
// 创建日期:		2016年5月30日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __CXX_11_BINDER_WRAPPER_H__
#define __CXX_11_BINDER_WRAPPER_H__

#define PLACE_HODLERS_0
#define PLACE_HODLERS_1 std::placeholders::_1
#define PLACE_HODLERS_2 PLACE_HODLERS_1, std::placeholders::_2
#define PLACE_HODLERS_3 PLACE_HODLERS_2, std::placeholders::_3
#define PLACE_HODLERS_4 PLACE_HODLERS_3, std::placeholders::_4
#define PLACE_HODLERS_5 PLACE_HODLERS_4, std::placeholders::_5
#define PLACE_HODLERS_6 PLACE_HODLERS_5, std::placeholders::_6
#define PLACE_HODLERS_7 PLACE_HODLERS_6, std::placeholders::_7
#define PLACE_HODLERS_8 PLACE_HODLERS_7, std::placeholders::_8
#define PLACE_HODLERS_9 PLACE_HODLERS_8, std::placeholders::_9
#define PLACE_HODLERS_10 PLACE_HODLERS_9, std::placeholders::_10
#define PLACE_HODLERS_11 PLACE_HODLERS_10, std::placeholders::_11
#define PLACE_HODLERS_12 PLACE_HODLERS_11, std::placeholders::_12
#define PLACE_HODLERS_13 PLACE_HODLERS_12, std::placeholders::_13
#define PLACE_HODLERS_14 PLACE_HODLERS_13, std::placeholders::_14
#define PLACE_HODLERS_15 PLACE_HODLERS_14, std::placeholders::_15
#define PLACE_HODLERS_16 PLACE_HODLERS_15, std::placeholders::_16
#define PLACE_HODLERS_17 PLACE_HODLERS_16, std::placeholders::_17
#define PLACE_HODLERS_18 PLACE_HODLERS_17, std::placeholders::_18
#define PLACE_HODLERS_19 PLACE_HODLERS_18, std::placeholders::_19
#define PLACE_HODLERS_20 PLACE_HODLERS_19, std::placeholders::_20

#define _PLACE_HODLERS(N) PLACE_HODLERS_##N
#define PLACE_HODLERS(N) _PLACE_HODLERS(N)

#define _fs_binder_x_this(ss, n, ...) std::bind(&ss, this, PLACE_HODLERS(n), ##__VA_ARGS__)
#define _fs_binder_x_(ss, n, ...) std::bind(&ss, PLACE_HODLERS(n), ##__VA_ARGS__)
#define _fs_binder_n(ss, n, x, ...) _fs_binder_x_##x(ss, n, ##__VA_ARGS__)

#define _fs_binder_x_0_this(ss, ...) std::bind(&ss, this, ##__VA_ARGS__)
#define _fs_binder_x_0_(ss, ...) std::bind(&ss, ##__VA_ARGS__)
#define _fs_binder_0(ss, x, ...) _fs_binder_x_0_##x(ss, ##__VA_ARGS__)

#define FS_BINDER_0(__selector__, ...) _fs_binder_0(__selector__, ##__VA_ARGS__)
#define FS_BINDER_1(__selector__, ...) _fs_binder_n(__selector__, 1, ##__VA_ARGS__)
#define FS_BINDER_2(__selector__, ...) _fs_binder_n(__selector__, 2, ##__VA_ARGS__)
#define FS_BINDER_3(__selector__, ...) _fs_binder_n(__selector__, 3, ##__VA_ARGS__)
#define FS_BINDER_4(__selector__, ...) _fs_binder_n(__selector__, 4, ##__VA_ARGS__)
#define FS_BINDER_5(__selector__, ...) _fs_binder_n(__selector__, 5, ##__VA_ARGS__)
#define FS_BINDER_6(__selector__, ...) _fs_binder_n(__selector__, 6, ##__VA_ARGS__)
#define FS_BINDER_7(__selector__, ...) _fs_binder_n(__selector__, 7, ##__VA_ARGS__)
#define FS_BINDER_8(__selector__, ...) _fs_binder_n(__selector__, 8, ##__VA_ARGS__)
#define FS_BINDER_9(__selector__, ...) _fs_binder_n(__selector__, 9, ##__VA_ARGS__)
#define FS_BINDER_10(__selector__, ...) _fs_binder_n(__selector__, 10, ##__VA_ARGS__)
#define FS_BINDER_11(__selector__, ...) _fs_binder_n(__selector__, 11, ##__VA_ARGS__)
#define FS_BINDER_12(__selector__, ...) _fs_binder_n(__selector__, 12, ##__VA_ARGS__)
#define FS_BINDER_13(__selector__, ...) _fs_binder_n(__selector__, 13, ##__VA_ARGS__)
#define FS_BINDER_14(__selector__, ...) _fs_binder_n(__selector__, 14, ##__VA_ARGS__)
#define FS_BINDER_15(__selector__, ...) _fs_binder_n(__selector__, 15, ##__VA_ARGS__)
#define FS_BINDER_16(__selector__, ...) _fs_binder_n(__selector__, 16, ##__VA_ARGS__)
#define FS_BINDER_17(__selector__, ...) _fs_binder_n(__selector__, 17, ##__VA_ARGS__)
#define FS_BINDER_18(__selector__, ...) _fs_binder_n(__selector__, 18, ##__VA_ARGS__)
#define FS_BINDER_19(__selector__, ...) _fs_binder_n(__selector__, 19, ##__VA_ARGS__)
#define FS_BINDER_20(__selector__, ...) _fs_binder_n(__selector__, 20, ##__VA_ARGS__)


#endif // END __CXX_11_BINDER_WRAPPER_H__