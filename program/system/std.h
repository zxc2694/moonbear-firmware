//儲存邊界數值限制相關函數

#ifndef STD_H
#define STD_H

#define Bound(_x, _min, _max) { if (_x > _max) _x = _max; else if (_x < _min) _x = _min; }
//將數值限制在max和min之間
#define BoundInverted(_x, _min, _max) {           \
		if ((_x < _min) && (_x > _max)) {             \
			if (abs(_x - _min) < abs(_x - _max))        \
				_x = _min;                                \
			else                                        \
				_x = _max;                                \
		}                                             \
	}

#define BoundAbs(_x, _max) Bound(_x, -(_max), (_max))

#endif

