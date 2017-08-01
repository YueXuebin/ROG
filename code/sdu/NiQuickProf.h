/*
 *	实时性能分析(来源于BulletEngine)
 */
#ifndef NIQUICKPROF_H
#define NIQUICKPROF_H

#ifdef WIN

#else
#define BT_NO_PROFILE       // 只在Windows下做性能分析
#endif


#ifndef BT_NO_PROFILE

//if you don't need btClock, you can comment next line
#define USE_BT_CLOCK 1

#ifdef USE_BT_CLOCK

#define mymin(a,b) (a > b ? a : b)

#define QUICKPROF_SAMPLE_KEEP_NUMBER		100		// 保留多少帧的采样数据

///The btClock is a portable basic clock that measures accurate time in seconds, use for profiling.
class btClock
{
public:
	btClock()
	{
		QueryPerformanceFrequency(&mClockFrequency);
		reset();
	}

	~btClock()
	{
	}

	/// Resets the initial reference time.
	void reset()
	{
		QueryPerformanceCounter(&mStartTime);
		mStartTick = GetTickCount();
		mPrevElapsedTime = 0;
	}

	/// Returns the time in ms since the last call to reset or since the btClock was created.
	unsigned long int getTimeMilliseconds()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		LONGLONG elapsedTime = currentTime.QuadPart - 
			mStartTime.QuadPart;

		// Compute the number of millisecond ticks elapsed.
		unsigned long msecTicks = (unsigned long)(1000 * elapsedTime / 
			mClockFrequency.QuadPart);

		// Check for unexpected leaps in the Win32 performance counter.  
		// (This is caused by unexpected data across the PCI to ISA 
		// bridge, aka south bridge.  See Microsoft KB274323.)
		unsigned long elapsedTicks = GetTickCount() - mStartTick;
		signed long msecOff = (signed long)(msecTicks - elapsedTicks);
		if (msecOff < -100 || msecOff > 100)
		{
			// Adjust the starting time forwards.
			LONGLONG msecAdjustment = mymin(msecOff * 
				mClockFrequency.QuadPart / 1000, elapsedTime - 
				mPrevElapsedTime);
			mStartTime.QuadPart += msecAdjustment;
			elapsedTime -= msecAdjustment;

			// Recompute the number of millisecond ticks elapsed.
			msecTicks = (unsigned long)(1000 * elapsedTime / 
				mClockFrequency.QuadPart);
		}

		// Store the current elapsed time for adjustments next time.
		mPrevElapsedTime = elapsedTime;

		return msecTicks;
	}

	/// Returns the time in us since the last call to reset or since the Clock was created.
	unsigned long int getTimeMicroseconds()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		LONGLONG elapsedTime = currentTime.QuadPart - 
			mStartTime.QuadPart;

		// Compute the number of millisecond ticks elapsed.
		unsigned long msecTicks = (unsigned long)(1000 * elapsedTime / 
			mClockFrequency.QuadPart);

		// Check for unexpected leaps in the Win32 performance counter.  
		// (This is caused by unexpected data across the PCI to ISA 
		// bridge, aka south bridge.  See Microsoft KB274323.)
		unsigned long elapsedTicks = GetTickCount() - mStartTick;
		signed long msecOff = (signed long)(msecTicks - elapsedTicks);
		if (msecOff < -100 || msecOff > 100)
		{
			// Adjust the starting time forwards.
			LONGLONG msecAdjustment = mymin(msecOff * 
				mClockFrequency.QuadPart / 1000, elapsedTime - 
				mPrevElapsedTime);
			mStartTime.QuadPart += msecAdjustment;
			elapsedTime -= msecAdjustment;
		}

		// Store the current elapsed time for adjustments next time.
		mPrevElapsedTime = elapsedTime;

		// Convert to microseconds.
		unsigned long usecTicks = (unsigned long)(1000000 * elapsedTime / 
			mClockFrequency.QuadPart);

		return usecTicks;
	}

private:
	LARGE_INTEGER mClockFrequency;
	DWORD mStartTick;
	LONGLONG mPrevElapsedTime;
	LARGE_INTEGER mStartTime;

};

#endif //USE_BT_CLOCK




///A node in the Profile Hierarchy Tree
class	CProfileNode {

public:
	CProfileNode( const char * name, CProfileNode * parent );
	~CProfileNode( void );

	CProfileNode * Get_Sub_Node( const char * name );

	CProfileNode * Get_Parent( void )		{ return Parent; }
	CProfileNode * Get_Sibling( void )		{ return Sibling; }
	CProfileNode * Get_Child( void )			{ return Child; }

	void				CleanupMemory();
	void				Reset( void );
	void				Call( void );
	bool				Return( void );

	const char *	Get_Name( void )				{ return Name; }
	int				Get_Total_Calls( void )		{ return TotalCalls; }
	float				Get_Total_Time( void )		{ return TotalTime; }
	float				Get_Call_Time( void )		{ return ThisCallTime; }

	// 保留的采样数据
	float			TimeKeep[QUICKPROF_SAMPLE_KEEP_NUMBER];
    int             CallsKeep[QUICKPROF_SAMPLE_KEEP_NUMBER];
	int				TimeKeepIndex;
protected:

	const char *	Name;
	int				TotalCalls;
	float			TotalTime;
	float			ThisCallTime;
	unsigned long int	StartTime;
	int				RecursionCounter;

	CProfileNode *	Parent;
	CProfileNode *	Child;
	CProfileNode *	Sibling;	
};

///An iterator to navigate through the tree
class CProfileIterator
{
public:
	// Access all the children of the current parent
	void				First(void);
	void				Next(void);
	bool				Is_Done(void);
	bool                Is_Root(void) { return (CurrentParent->Get_Parent() == 0); }

	void				Enter_Child( int index );		// Make the given child the new parent
	void				Enter_Largest_Child( void );	// Make the largest child the new parent
	void				Enter_Parent( void );			// Make the current parent's parent the new parent

	// Access the current child
	const char *	Get_Current_Name( void )			{ return CurrentChild->Get_Name(); }
	int				Get_Current_Total_Calls( void )	{ return CurrentChild->Get_Total_Calls(); }
	float			Get_Current_Total_Time( void )	{ return CurrentChild->Get_Total_Time(); }
	float			Get_Current_Call_Time()			{ return CurrentChild->Get_Call_Time(); }

	// Access the current parent
	const char *	Get_Current_Parent_Name( void )			{ return CurrentParent->Get_Name(); }
	int				Get_Current_Parent_Total_Calls( void )	{ return CurrentParent->Get_Total_Calls(); }
	float				Get_Current_Parent_Total_Time( void )	{ return CurrentParent->Get_Total_Time(); }

protected:

	CProfileNode *	CurrentParent;		// 当前Node
	CProfileNode *	CurrentChild;

	CProfileIterator( CProfileNode * start );
	friend	class		CProfileManager;
};


///The Manager for the Profile system
class	CProfileManager {
public:
	static	void						Start_Profile( const char * name );
	static	void						Stop_Profile( void );

	static	void						CleanupMemory(void)
	{
		Root.CleanupMemory();
	}

	static	void						Reset( void );
	static	void						Increment_Frame_Counter( void );
	static	int							Get_Frame_Count_Since_Reset( void )		{ return FrameCounter; }
	static	float						Get_Time_Since_Reset( void );

	static	CProfileIterator *	Get_Iterator( void )	
	{ 

		return new CProfileIterator( &Root ); 
	}
	static	void						Release_Iterator( CProfileIterator * iterator ) { delete ( iterator); }

	static CProfileNode*	GetRootNode()
	{
		return &Root;
	}

	static	bool	Stopped;
	//static void	dumpRecursive(CProfileIterator* profileIterator, int spacing);

	//static void	dumpAll();

private:
	static	CProfileNode			Root;
	static	CProfileNode *			CurrentNode;
	static	int						FrameCounter;
	static	unsigned long int		ResetTime;
};


///ProfileSampleClass is a simple way to profile a function's scope
///Use the BT_PROFILE macro at the start of scope to time
class	CProfileSample {
public:
	CProfileSample( const char * name )
	{ 
		CProfileManager::Start_Profile( name ); 
	}

	~CProfileSample( void )					
	{ 
		CProfileManager::Stop_Profile(); 
	}
};


#define	BT_PROFILE( name )			CProfileSample __profile( name )
#define BT_PROFILE_FUNC				BT_PROFILE(__FUNCTION__)

#else

#define	BT_PROFILE( name )
#define BT_PROFILE_FUNC

#endif //#ifndef BT_NO_PROFILE

#endif // NIQUICKPROF_H