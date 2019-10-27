#ifndef UTIL_H_
#define UTIL_H_

//デリートを楽に書くためのやつ

template <typename T>
inline void safe_delete(T*& p)
{
	if (p != nullptr)
	{
		delete (p);
		(p) = nullptr;
	}
}

template <typename T>
inline void safe_delete_array(T*& p)
{
	if (p != nullptr)
	{
		delete[](p);
		(p) = nullptr;
	}
}

template <typename T>
inline void safe_release(T*& p)
{
	if (p != nullptr)
	{
		(p)->Release();
		(p) = nullptr;
	}
}

#endif // !UTIL_H_
