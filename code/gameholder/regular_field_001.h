#ifndef regular_field_001_h__
#define regular_field_001_h__


#include "regular_field_base.h"

// 野外1
class RegularField_001 : public RegularFieldBase
{
public:
    RegularField_001(RegularDoc* pDoc);
    virtual ~RegularField_001();

    virtual void FirstUpdate();


};


#endif // regular_field_001_h__
