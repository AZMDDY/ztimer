#ifndef ZTIMER_ERROR_CODE_H_
#define ZTIMER_ERROR_CODE_H_

namespace ztimer {
    const int ERR_OK = 0;                          // 执行成功
    const int ERR_TIMER_ID_INVALID_EXIST = 1;      // 定时器ID无效或已存在
    const int ERR_TIME_POINT_LATE = 2;             // 绝对定时器时间点晚于当前系统时间
    const int ERR_TIME_POINT_FMT = 3;              // 绝对定时器时间点格式错误
    const int ERR_TIMER_ID_INVALID_NOT_EXIST = 4;  // 定时器ID无效或不存在
}  // namespace ztimer

#endif  // ZTIMER_ERROR_CODE_H_