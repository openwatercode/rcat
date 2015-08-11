#pragma once

// 단위 변경은 아래의 매크로를 수정하면 됨.
// MMH2MS(x) (x) 로 하는 경우 단위 변환이 일어 나지 않음.

// mm를 m로 변경
#define MM2M(x) (x / 1000)
// m를 mm로 변경
#define M2MM(x) (x * 1000)
// mm/h를 m/s로 변경
//#define MMH2MS(x) (x / 3600000)
#define MMH2MS(x) (x / 1000)
// m/s를 mm/h로 변경
//#define MS2MMH(x) (x * 3600000)
#define MS2MMH(x) (x * 1000)
// m2를 km2으로 변경
#define M22KM2(x) (x / 1000000)
// km2를 m2로 변경
#define KM22M2(x) (x * 1000000)

