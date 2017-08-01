// 实现Rtti功能的类(copyfrom Gamebryo)
#ifndef Rtti_h__
#define Rtti_h__

class CnRTTI
{
public:
	CnRTTI (const char* pcName, const CnRTTI* pkBaseRTTI);

	inline const char* GetName() const {return m_pcName;}
	inline const CnRTTI* GetBaseRTTI() const {return m_pkBaseRTTI;}

	bool CopyName(char* acNameBuffer, unsigned int uiMaxSize) const;

protected:
	const char* m_pcName;
	const CnRTTI* m_pkBaseRTTI;
};

// insert in root class declaration
#define CnDeclareRootRTTI(classname) \
	public: \
	static const CnRTTI ms_RTTI; \
	virtual const CnRTTI* GetRTTI() const {return &ms_RTTI;} \
	static bool IsExactKindOf(const CnRTTI* pkRTTI, \
		const classname* pkObject) \
	{ \
		if (!pkObject) \
		{ \
			return false; \
		} \
		return pkObject->IsExactKindOf(pkRTTI); \
	} \
	bool IsExactKindOf(const CnRTTI* pkRTTI) const \
	{ \
		return (GetRTTI() == pkRTTI); \
	} \
	static bool IsKindOf(const CnRTTI* pkRTTI, \
		const classname* pkObject) \
	{ \
		if (!pkObject) \
		{ \
			return false; \
		} \
		return pkObject->IsKindOf(pkRTTI); \
	} \
	bool IsKindOf(const CnRTTI* pkRTTI) const \
	{ \
		const CnRTTI* pkTmp = GetRTTI(); \
		while (pkTmp) \
		{ \
			if (pkTmp == pkRTTI) \
			{ \
				return true; \
			} \
			pkTmp = pkTmp->GetBaseRTTI(); \
		} \
		return false; \
	} \
	static classname* VerifyStaticCastDebug(const CnRTTI* pkRTTI, \
		const classname* pkObject) \
	{ \
		if (!pkObject) \
		{ \
			return NULL; \
		} \
		classname* pkDynamicCast = DynamicCast(pkRTTI, pkObject); \
		CnVerify("NiVerifyStaticCast() caught an invalid type cast." \
			"Check callstack for invalid object typecast assumption." \
			&& pkDynamicCast); \
		return pkDynamicCast; \
	} \
	static classname* DynamicCast(const CnRTTI* pkRTTI, \
		const classname* pkObject) \
	{ \
		if (!pkObject) \
		{ \
			return NULL; \
		} \
		return pkObject->DynamicCast(pkRTTI); \
	} \
	classname* DynamicCast(const CnRTTI* pkRTTI) const \
	{ \
		return (IsKindOf(pkRTTI) ? (classname*) this : 0 ); \
	}

// insert in class declaration
#define CnDeclareRTTI \
	public: \
	static const CnRTTI ms_RTTI; \
	virtual const CnRTTI* GetRTTI() const {return &ms_RTTI;}

// insert in root class source file
#define CnImplementRootRTTI(rootclassname) \
	const CnRTTI rootclassname::ms_RTTI(#rootclassname, 0)

// insert in class source file
#define CnImplementRTTI(classname, baseclassname) \
	const CnRTTI classname::ms_RTTI(#classname, &baseclassname::ms_RTTI)

// macros for run-time type testing
#define CnIsExactKindOf(classname, pkObject) \
	classname::IsExactKindOf(&classname::ms_RTTI, pkObject)

#define CnIsKindOf(classname, pkObject) \
	classname::IsKindOf(&classname::ms_RTTI, pkObject)

// macro for compile time type casting
#define CnStaticCast(classname, pkObject) \
	((classname*) pkObject)

// macro for compile time type casting, with debug run-time assert
#ifdef _DEBUG
#define CnVerifyStaticCast(classname, pkObject) \
		((classname*) classname::VerifyStaticCastDebug( \
			&classname::ms_RTTI, pkObject))
#else
#define CnVerifyStaticCast(classname, pkObject) ((classname*) (pkObject))
#endif

// macro for run-time type casting, returns NULL if invalid cast
#define CnDynamicCast(classname, pkObject) \
	(static_cast<classname*>(classname::DynamicCast(&classname::ms_RTTI, pkObject)))

#endif // Rtti_h__
