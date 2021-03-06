#ifndef FSTDATASCALERTO01_H
#define FSTDATASCALERTO01_H

/*!======================================================================
   Feature Selection Toolbox 3 source code
   ---------------------------------------
	
   \file    data_scaler_to01.hpp
   \brief   Implements data normalization (of all feature values) to interval [0,1]
   \author  Petr Somol (somol@utia.cas.cz) with collaborators, see Contacts at http://fst.utia.cz
   \date    March 2011
   \version 3.1.0.beta
   \note    FST3 was developed using gcc 4.3 and requires
   \note    \li Boost library (http://www.boost.org/, tested with versions 1.33.1 and 1.44),
   \note    \li (\e optionally) LibSVM (http://www.csie.ntu.edu.tw/~cjlin/libsvm/, 
                tested with version 3.00)
   \note    Note that LibSVM is required for SVM related tools only,
            as demonstrated in demo12t.cpp, demo23.cpp, demo25t.cpp, demo32t.cpp, etc.

*/ /* 
=========================================================================
Copyright:
  * FST3 software (with exception of any externally linked libraries) 
    is copyrighted by Institute of Information Theory and Automation (UTIA), 
    Academy of Sciences of the Czech Republic.
  * FST3 source codes as presented here do not contain code of third parties. 
    FST3 may need linkage to external libraries to exploit its functionality
    in full. For details on obtaining and possible usage restrictions 
    of external libraries follow their original sources (referenced from
    FST3 documentation wherever applicable).
  * FST3 software is available free of charge for non-commercial use. 
    Please address all inquires concerning possible commercial use 
    of FST3, or if in doubt, to FST3 maintainer (see http://fst.utia.cz).
  * Derivative works based on FST3 are permitted as long as they remain
    non-commercial only.
  * Re-distribution of FST3 software is not allowed without explicit
    consent of the copyright holder.
Disclaimer of Warranty:
  * FST3 software is presented "as is", without warranty of any kind, 
    either expressed or implied, including, but not limited to, the implied 
    warranties of merchantability and fitness for a particular purpose. 
    The entire risk as to the quality and performance of the program 
    is with you. Should the program prove defective, you assume the cost 
    of all necessary servicing, repair or correction.
Limitation of Liability:
  * The copyright holder will in no event be liable to you for damages, 
    including any general, special, incidental or consequential damages 
    arising out of the use or inability to use the code (including but not 
    limited to loss of data or data being rendered inaccurate or losses 
    sustained by you or third parties or a failure of the program to operate 
    with any other programs).
========================================================================== */

#include <iostream>
#include <cmath>
#include "error.hpp"
#include "global.hpp"
#include "data_scaler.hpp"

/*============== Template parameter type naming conventions ==============
--------- Numeric types: -------------------------------------------------
DATATYPE - data sample values - usually real numbers (but may be integers
          in text processing etc.)
REALTYPE - must be real numbers - for representing intermediate results of 
          calculations like mean, covariance etc.
IDXTYPE - index values for enumeration of data samples - (nonnegative) integers, 
          extent depends on numbers of samples in data
DIMTYPE - index values for enumeration of features (dimensions), or classes (not 
          class sizes) - (nonnegative) integers, usually lower extent than IDXTYPE, 
          but be aware of expressions like _classes*_features*_features ! 
          in linearized representations of feature matrices for all classes
BINTYPE - feature selection marker type - represents ca. <10 different feature 
          states (selected, deselected, sel./desel. temporarily 1st nested loop, 2nd...)
RETURNTYPE - criterion value: real value, but may be extended in future to support 
          multiple values 
--------- Class types: ---------------------------------------------------
SUBSET       - class of class type Subset 
CLASSIFIER   - class implementing interface defined in abstract class Classifier 
EVALUATOR    - class implementing interface defined in abstract class Sequential_Step 
DISTANCE     - class implementing interface defined in abstract class Distance 
DATAACCESSOR - class implementing interface defined in abstract class Data_Accessor 
INTERVALCONTAINER - class of class type TIntervaller 
CONTAINER    - STL container of class type TInterval  
========================================================================== */

namespace FST {

/*! \brief Implements data normalization (of all feature values) to interval [0,1]

    \note this implementation supports only scaling of one-dimensional data,
       thus is applicable only to scale feature values individually 
       and independently for each other feature

	\note optionally substitutes missing values by the mean of those values that are available (separately per feature).
    Missing values are assumed to be coded by dedicated numerical value 'missing_val_code'. 
*/
template<typename DATATYPE>
class Data_Scaler_to01 : public Data_Scaler<DATATYPE> {
public:
	Data_Scaler_to01(const int dims=1): first_learn(true), missing_values(false), _missing_val_code(0) {if(dims!=1) throw fst_error("Data_Scaler_to01() implemented for 1-dimensional data only."); notify("Data_Scaler_to01 constructor.");}
	Data_Scaler_to01(const DATATYPE missing_val_code, const int dims): first_learn(true), missing_values(true), _missing_val_code(missing_val_code) {if(dims!=1) throw fst_error("Data_Scaler_to01() implemented for 1-dimensional data only."); notify("Data_Scaler_to01 constructor.");}
	Data_Scaler_to01(const Data_Scaler_to01 &ds) : first_learn(ds.first_learn), min(ds.min), max(ds.max), missing_values(ds.missing_values), _missing_val_code(ds._missing_val_code), count(ds.count), sum(ds.sum), avg(ds.avg) {notify("Data_Scaler_to01 copy constructor.");}
	virtual ~Data_Scaler_to01() {notify("Data_Scaler_to01 destructor.");}
	virtual int learn_loops() const {return 1;}
	virtual bool startFirstLoop() {max=min=0; first_learn=true; sum=0; count=0; return true;}
	virtual bool startNextLoop() {return false;}
	virtual void learn(const DATATYPE& value);

	virtual DATATYPE scale(const DATATYPE& value); //!< return the scaled value
	virtual void scale_inplace(DATATYPE& value); //!< scale the value in place
	
	virtual std::ostream& print(std::ostream& os) const {os << "Data_Scaler_to01("; if(missing_values) os << "missing_value=" << _missing_val_code; os << ")"; return os;}
	
protected:
	bool first_learn;
	DATATYPE min;
	DATATYPE max;

	// missing value substitution support
	bool missing_values;
	const DATATYPE _missing_val_code;
	long count;
	DATATYPE sum;
	DATATYPE avg;
};

template<typename DATATYPE>
void Data_Scaler_to01<DATATYPE>::learn(const DATATYPE& value)
{
	if(!missing_values || value!=_missing_val_code) {
		if(first_learn) {min=max=value; first_learn=false;} else {
			if(value>max) max=value;
			if(value<min) min=value;
		}
		sum+=value; count++;
		avg=sum/(DATATYPE)count;
	}
}

template<typename DATATYPE>
DATATYPE Data_Scaler_to01<DATATYPE>::scale(const DATATYPE& value) 
{
	if(count==0) // all values missing
		return 0;
	else if(missing_values && value==_missing_val_code)
	{
		if(max>min) return (avg-min)/(max-min); else return 0;
	} else {
		if(max>min) return (value-min)/(max-min); else return 0;
	}
}
	
template<typename DATATYPE>
void Data_Scaler_to01<DATATYPE>::scale_inplace(DATATYPE& value) 
{
	if(count==0) // all values missing
		value=0;
	else if(missing_values && value==_missing_val_code)
	{
		if(max>min) value=(avg-min)/(max-min); else value=0;
	} else {
		if(max>min) value=(value-min)/(max-min); else value=0;
	}
}

} // namespace
#endif // FSTDATASCALERTO01_H ///:~
