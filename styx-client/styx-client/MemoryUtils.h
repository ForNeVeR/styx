#pragma once

#include <memory>

struct MemoryUtils
{
	template<class TObject>
	static std::unique_ptr<TObject> MakeUniquePtr(TObject *pointer)
	{
		return std::unique_ptr(pointer);
	}
	
	template<class TObject, class TDeleter>
	static std::unique_ptr<TObject, TDeleter> MakeUniquePtr(TObject *pointer, TDeleter deleter = TDeleter())
	{
		return std::unique_ptr<TObject, TDeleter>(pointer, deleter);
	}
};