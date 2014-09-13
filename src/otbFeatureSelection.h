/*=========================================================================
 Program:   ORFEO Toolbox
 Language:  C++
 Date:      $Date$
 Version:   $Revision$
 Created By:Bouceffa Walid
 Email:	    bouceffa.walid@gmail.com


 Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
 See OTBCopyright.txt for details.


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/

#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

#include <iostream>
#include <stdio.h>
#include "otbConfigurationFile.h"

//Image
#include "otbImage.h"
#include "otbImageFileWriter.h"
#include "otbVectorImage.h"
#include "otbVectorData.h"
#include "otbListSampleGenerator.h"
#include "otbMultiChannelExtractROI.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbMultiToMonoChannelExtractROI.h"

// ListSample
#include "itkListSample.h"
#include "itkVariableLengthVector.h"
#include "itkFixedArray.h"

//Estimator
#include "otbMachineLearningModelFactory.h"

#ifdef OTB_USE_OPENCV
# include "otbRandomForestsMachineLearningModel.h"
#endif

#include "otbLibSVMMachineLearningModel.h"

// Statistic XML Reader
#include "otbStatisticsXMLFileReader.h"

// Validation
#include "otbConfusionMatrixCalculator.h"

#include "itkTimeProbe.h"
#include "otbStandardFilterWatcher.h"

// Normalize the samples
#include "otbShiftScaleSampleListFilter.h"

// List sample concatenation
#include "otbConcatenateSampleListFilter.h"

// Balancing ListSample
#include "otbListSampleToBalancedListSampleFilter.h"

// VectorData projection filter
#include "otbVectorDataProjectionFilter.h"

// Extract a ROI of the vectordata
#include "otbVectorDataIntoImageProjectionFilter.h"

// Elevation handler
#include "otbWrapperElevationParametersHandler.h"


//ExtractROI
#include "otbMultiChannelExtractROI.h"

namespace otb
{
namespace Wrapper
{

class FeatureSelection: public Application
{
public:
    /** Standard class typedefs. */
    typedef FeatureSelection Self;
    typedef Application Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    /** Standard macro */
    itkNewMacro ( Self )

    itkTypeMacro ( FeatureSelection, otb::Application )



    //vector importance
    typedef std::pair<int,float>				ImportancePairType;
    typedef std::vector<ImportancePairType>		ImportanceVector;

    typedef FloatVectorImageType::PixelType         PixelType;
    typedef FloatVectorImageType::InternalPixelType InternalPixelType;

    // Training vectordata
    typedef itk::VariableLengthVector<InternalPixelType> MeasurementType;

    // SampleList manipulation
    typedef otb::ListSampleGenerator<FloatVectorImageType, VectorDataType> ListSampleGeneratorType;



    typedef ListSampleGeneratorType::ListSampleType ListSampleType;
    typedef ListSampleGeneratorType::LabelType LabelType;
    typedef ListSampleGeneratorType::ListLabelType LabelListSampleType;
    typedef otb::Statistics::ConcatenateSampleListFilter<ListSampleType> ConcatenateListSampleFilterType;
    typedef otb::Statistics::ConcatenateSampleListFilter<LabelListSampleType> ConcatenateLabelListSampleFilterType;

    // Statistic XML file Reader
    typedef otb::StatisticsXMLFileReader<MeasurementType> StatisticsReader;

    // Enhance List Sample  typedef otb::Statistics::ListSampleToBalancedListSampleFilter<ListSampleType, LabelListSampleType>      BalancingListSampleFilterType;
    typedef otb::Statistics::ShiftScaleSampleListFilter<ListSampleType, ListSampleType> ShiftScaleFilterType;

    // Machine Learning models
    typedef otb::MachineLearningModelFactory<InternalPixelType, ListSampleGeneratorType::ClassLabelType> MachineLearningModelFactoryType;
    typedef MachineLearningModelFactoryType::MachineLearningModelTypePointer ModelPointerType;

#ifdef OTB_USE_OPENCV
    typedef otb::RandomForestsMachineLearningModel<InternalPixelType, ListSampleGeneratorType::ClassLabelType> RandomForestType;
#endif

    // VectorData projection filter
    typedef otb::VectorDataProjectionFilter<VectorDataType, VectorDataType> VectorDataProjectionFilterType;

    // Extract ROI
    typedef otb::VectorDataIntoImageProjectionFilter<VectorDataType, FloatVectorImageType> VectorDataReprojectionType;




    /** Filters typedef */
    typedef otb::MultiChannelExtractROI<FloatVectorImageType::InternalPixelType,
            FloatVectorImageType::InternalPixelType> ExtractROIFilterType;




protected:
    using Superclass::AddParameter;

private:
    void DoInit();

#ifdef OTB_USE_OPENCV
    void InitRandomForestsParams();
    ImportanceVector getFeatureImportanceRandomForests ( ListSampleType::Pointer trainingListSample,
            LabelListSampleType::Pointer trainingLabeledListSample );
#endif

    void InitFSTParams();

    std::vector<std::string> getBandList ( std::vector<std::string> images );
    
    
    ImportanceVector getFeatureImportanceDOS ( ListSampleType::Pointer trainingListSample,
            LabelListSampleType::Pointer trainingLabeledListSample );    
   
    ImportanceVector getFeatureImportanceSFFS ( ListSampleType::Pointer trainingListSample,
            LabelListSampleType::Pointer trainingLabeledListSample );
    

    ImportanceVector getFeatureImportance ( ListSampleType::Pointer trainingListSample, LabelListSampleType::Pointer trainingLabeledListSample );

    void DoUpdateParameters();

    void DoExecute();


    typedef UInt16ImageType        OutputImageType;

    typedef otb::ImageList<FloatImageType>  	ImageListType;
    typedef ImageListToVectorImageFilter<ImageListType, FloatVectorImageType > 		ListConcatenerFilterType;


    typedef MultiToMonoChannelExtractROI<FloatVectorImageType::InternalPixelType,
            FloatImageType::PixelType>               MMExtractROIFilterType;


    typedef ObjectList<MMExtractROIFilterType>                                      ExtractROIFilterListType;

    FloatVectorImageListType::Pointer 			m_inList;
    ListConcatenerFilterType::Pointer 			m_Concatener;
    ExtractROIFilterListType::Pointer  			m_ExtractorList;
    ImageListType::Pointer 				m_ImageList;



    VectorDataReprojectionType::Pointer vdreproj;

// ExtractROIFilterType::Pointer   extractROIFilter;

    std::vector<std::string>  imagesBandList;

};

}
}


