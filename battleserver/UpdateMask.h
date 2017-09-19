#pragma once
#ifndef __UPDATEMASK_H
#define __UPDATEMASK_H
#include "common_type.h"
#include "assert.h"
class UpdateMask
{
	u32 *mUpdateMask;
	u32 mCount; // in values
	u32 mBlocks; // in u32 blocks

public:
	UpdateMask() : mUpdateMask(0), mCount(0), mBlocks(0) { }
	UpdateMask(const UpdateMask& mask) : mUpdateMask(0) { *this = mask; }

	~UpdateMask()
	{
		if (mUpdateMask)
			delete[] mUpdateMask;
	}

	void SetBit(const u32 index)
	{
		assert(index < mCount);
		((u8 *)mUpdateMask)[index >> 3] |= 1 << (index & 0x7);
		// ( (u8 *)mUpdateMask )[ index / 8 ] |= 1 * pow( 2, index % 8 );
	}

	void UnsetBit(const u32 index)
	{
		assert(index < mCount);
		((u8 *)mUpdateMask)[index >> 3] &= (0xff ^ (1 << (index & 0x7)));
		// ( (u8 *)mUpdateMask )[ index / 8 ] &= 255 - ( 1 * pow( 2, index % 8 ) ) );
	}

	bool GetBit(const u32 index) const
	{
		if (index < mCount)
			return (((u8 *)mUpdateMask)[index >> 3] & (1 << (index & 0x7))) != 0;
		else
			return false;
		//actually int->bool conversion is not needed here
	}

	u32 GetUpdateBlockCount() const
	{
		u32 x;
		for (x = mBlocks - 1; x; x--)
			if (mUpdateMask[x])break;
		return (x + 1);
	}
	inline u32 GetBlockCount() const { return mBlocks; }

	inline u32 GetLength() const { return (mBlocks *sizeof(u32)); }
	inline u32 GetCount() const { return mCount; }
	inline const u8* GetMask() const { return (u8*)mUpdateMask; }

	void SetCount(u32 valuesCount)
	{
		if (mUpdateMask)
			delete[] mUpdateMask;

		mCount = valuesCount;
		//mBlocks = valuesCount/32 + 1;
		//mBlocks = (valuesCount + 31) / 32;
		mBlocks = mCount >> 5;
		if (mCount & 31)
			++mBlocks;

		mUpdateMask = new u32[mBlocks];
		memset(mUpdateMask, 0, mBlocks *sizeof(u32));
	}

	void Clear()
	{
		if (mUpdateMask)
			memset(mUpdateMask, 0, mBlocks << 2);
	}

	UpdateMask& operator = (const UpdateMask& mask)
	{
		SetCount(mask.mCount);
		memcpy(mUpdateMask, mask.mUpdateMask, mBlocks << 2);

		return *this;
	}

	void operator &= (const UpdateMask& mask)
	{
		assert(mask.mCount <= mCount);
		for (u32 i = 0; i < mBlocks; i++)
			mUpdateMask[i] &= mask.mUpdateMask[i];
	}

	void operator |= (const UpdateMask& mask)
	{
		assert(mask.mCount <= mCount);
		for (u32 i = 0; i < mBlocks; i++)
			mUpdateMask[i] |= mask.mUpdateMask[i];
	}

	UpdateMask operator & (const UpdateMask& mask) const
	{
		assert(mask.mCount <= mCount);

		UpdateMask newmask;
		newmask = *this;
		newmask &= mask;

		return newmask;
	}

	UpdateMask operator | (const UpdateMask& mask) const
	{
		assert(mask.mCount <= mCount);

		UpdateMask newmask;
		newmask = *this;
		newmask |= mask;

		return newmask;
	}
};

#endif
