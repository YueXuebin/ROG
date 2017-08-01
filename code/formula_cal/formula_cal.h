#ifndef FORMULA_CAL_H
#define FORMULA_CAL_H

//class FormulaTemplate;
//class FormulaObject;

#include "formula_template.h"
#include "formula_parameter.h"
#include "formula_parser.h"

class FormulaCal
{
public:
    FormulaCal();
    ~FormulaCal();

    DECLARE_SINGLETON(FormulaCal)

    real CalResult(const FormulaTemplate* formulaTmpl, FormulaParameter* param);

    real CalResult(const FormulaTemplate* formulaTmpl, FormulaObject* aObj=NULL, FormulaObject* bObj=NULL);

private:
    real Calculate(FormulaTemplate& formulaTmpl);
};

#endif
