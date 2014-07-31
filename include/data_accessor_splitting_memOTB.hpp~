#ifndef FSTDATAACCESSORSPLITTINGMEMOTB_H
#define FSTDATAACCESSORSPLITTINGMEMOTB_H
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include "error.hpp"
#include "global.hpp"
#include "data_accessor_splitting_mem.hpp"

#include "itkListSample.h"

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


void notify(const string msg)
{
	//std::cout << msg << std::endl;
}

//! Implements data access to data cached entirely in memory, read once from OTB training DATA
template<typename DATATYPE, typename IDXTYPE, class INTERVALCONTAINER>
class Data_Accessor_Splitting_MemOTB : public Data_Accessor_Splitting_Mem<DATATYPE,IDXTYPE,INTERVALCONTAINER>
{
	// NOTE: this modified version unifies calling conventions for both inner and outer CV loops
	// - in both loops the same methods are to be called (just get*Split, get*Block, get*Pattern), while
	//   the actual inner/outer mode is to be switched on/off by 
	public:
		typedef Data_Accessor_Splitting_Mem<DATATYPE,IDXTYPE,INTERVALCONTAINER> DASM;
		typedef boost::shared_ptr<Data_Scaler<DATATYPE> > PScaler;
		typedef typename DASM::PSplitters PSplitters;
		
	  // Input related typedefs
  	typedef itk::VariableLengthVector<DATATYPE>     SampleType;
		typedef itk::Statistics::ListSample<SampleType> ListSampleType;
		typedef typename ListSampleType::Pointer        ListSamplePointerType;

		/** List to store the corresponding labels */
		typedef IDXTYPE                                ClassLabelType;
		typedef itk::FixedArray<ClassLabelType, 1>     LabelType;  //note could be templated by an std:::string
		typedef itk::Statistics::ListSample<LabelType> ListLabelType;
		typedef typename ListLabelType::Pointer        ListLabelPointerType;
		//
		Data_Accessor_Splitting_MemOTB(ListSamplePointerType _ls, ListLabelPointerType _ll , const PSplitters _dsp, const PScaler _dsc) : Data_Accessor_Splitting_Mem<DATATYPE,IDXTYPE,INTERVALCONTAINER>("OTB", _dsp, _dsc), ls(_ls),ll(_ll) {
		notify("Data_Accessor_Splitting_MemOTB constructor.");
		}
		virtual ~Data_Accessor_Splitting_MemOTB(){notify("Data_Accessor_Splitting_MemOTB destructor.");}
		
		Data_Accessor_Splitting_MemOTB* sharing_clone() const;
		
		virtual std::ostream& print(std::ostream& os) const;
		protected:
		Data_Accessor_Splitting_MemOTB(const Data_Accessor_Splitting_MemOTB &damt, int x) : Data_Accessor_Splitting_Mem<DATATYPE,IDXTYPE,INTERVALCONTAINER>(damt, x) {}; // weak (referencing) copy-constructor to be used in sharing_clone()
		protected:
		ListSamplePointerType ls;
		ListLabelPointerType ll;
		unsigned int noClasses;	
		unsigned int noFeatures;	
		std::vector<IDXTYPE> vClassesSizes;	
		std::map<ClassLabelType, unsigned int> classesCount;
		
		virtual void initial_file_prepare() ;    //!< \note off-limits in shared_clone
		virtual void initial_data_read() ;    //!< \note off-limits in shared_clone
		
    public:
		virtual unsigned int file_getNoOfClasses() const {return noClasses; };
		virtual unsigned int file_getNoOfFeatures() const {return noFeatures; };
		virtual IDXTYPE file_getClassSize(unsigned int cls) const {if(cls<noClasses) return vClassesSizes[cls]; else return 0; };

};

template<typename DATATYPE, typename IDXTYPE, class INTERVALCONTAINER>
void Data_Accessor_Splitting_MemOTB<DATATYPE,IDXTYPE,INTERVALCONTAINER>::initial_file_prepare() //!< \note off-limits in shared_clone
{
	std::cout << "initial_file_prepare" << std::endl;	
	
	for(unsigned int i =0; i<ll->Size(); i++)
		++classesCount[ll->GetMeasurementVector(i).GetElement(0)];

	noClasses = classesCount.size();
	noFeatures = ls->GetMeasurementVector(0).GetSize();
	std::cout << "noClasses : " << noClasses << std::endl;
	std::cout << "noFeatures : " << noFeatures << std::endl;
	for (typename std::map<ClassLabelType, unsigned int>::iterator it = classesCount.begin(); it != classesCount.end(); ++it)
		vClassesSizes.push_back(it->second);

}

template<typename DATATYPE, typename IDXTYPE, class INTERVALCONTAINER>
void Data_Accessor_Splitting_MemOTB<DATATYPE,IDXTYPE,INTERVALCONTAINER>::initial_data_read() //!< \note off-limits in shared_clone
{
	
	std::cout << "initial_data_read" << std::endl;	
		
	if(Clonable::is_sharing_clone()) throw fst_error("Data_Accessor_Splitting_MemOTB()::initial_data_read() called from shared_clone instance.");
	{std::ostringstream sos; sos << "Reading data to memory..."; syncout::print(std::cout,sos);}
	
	IDXTYPE idx=0;
	//We've got all the classes, then we reorder the data classe by classe
  for (typename std::map<ClassLabelType, unsigned int>::iterator it = classesCount.begin(); it != classesCount.end(); ++it)
	{
		std::cout << "Classe : " << it->first << ", Size : " << it->second << std::endl;
		for(unsigned int i =0; i<ll->Size(); i++)
		{
			
			if(ll->GetMeasurementVector(i).GetElement(0) == it->first)
			{
				for(unsigned int j=0;j<ls->GetMeasurementVector(i).GetSize();j++)
				{
					this->data[idx++]=ls->GetMeasurementVector(i).GetElement(j);
				}
			}
		}
	}
	
	{std::ostringstream sos; sos << "done."<<std::endl; syncout::print(std::cout,sos);}
}

/*! \warning A sharing_clone() object shares 'data' with the original object. Although the sharing_clone
             object can not modify 'data', there is no synchronization implemented to avoid
             concurrent modification of 'data' by the original object while reading it from the sharing_clone! */
template<typename DATATYPE, typename IDXTYPE, class INTERVALCONTAINER>
Data_Accessor_Splitting_MemOTB<DATATYPE,IDXTYPE,INTERVALCONTAINER>* Data_Accessor_Splitting_MemOTB<DATATYPE,IDXTYPE,INTERVALCONTAINER>::sharing_clone() const
{
	Data_Accessor_Splitting_MemOTB<DATATYPE,IDXTYPE,INTERVALCONTAINER> *clone=new Data_Accessor_Splitting_MemOTB<DATATYPE,IDXTYPE,INTERVALCONTAINER>(*this, (int)0);
	clone->set_sharing_cloned();
	return clone;
}

template<typename DATATYPE, typename IDXTYPE, class INTERVALCONTAINER>
std::ostream& Data_Accessor_Splitting_MemOTB<DATATYPE,IDXTYPE,INTERVALCONTAINER>::print(std::ostream& os) const 
{
	DASM::print(os); 
	os << std::endl << "Data_Accessor_Splitting_MemOTB()";
	return os;
}
	
} // namespace
#endif // FSTDATAACCESSORSPLITTINGMEMOTB_H ///:~
