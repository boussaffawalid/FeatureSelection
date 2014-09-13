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

#include "otbFeatureSelection.h"

namespace otb
{
namespace Wrapper
{

void FeatureSelection::DoInit()
{
    SetName ( "FeatureSelection" );
    SetDescription (
        "Feature selection from multiple images and training vector data." );

    // Documentation
    SetDocName ( "Feature selection from multiple images" );
    SetDocLongDescription ( "" );
    SetDocLimitations ( "None" );
    SetDocAuthors ( "Bouceffa Walid" );

    AddDocTag ( Tags::Learning );

    //Group IO
    AddParameter ( ParameterType_Group, "io", "Input and output data" );
    SetParameterDescription ( "io", "This group of parameters allows to set input and output data." );

    AddParameter ( ParameterType_InputImageList, "io.il", "Input Image List" );
    SetParameterDescription ( "io.il", "A list of input images." );

    AddParameter ( ParameterType_InputVectorDataList, "io.vd", "Input Vector Data" );
    SetParameterDescription ( "io.vd", " vector data to select the training samples." );

    AddParameter ( ParameterType_InputFilename, "io.imstat", "Input XML image statistics file" );
    MandatoryOff ( "io.imstat" );
    SetParameterDescription ( "io.imstat",
                              "Input XML file containing the mean and the standard deviation of the input images." );


    AddParameter ( ParameterType_OutputImage, "io.out",  "Output Image" );
    SetParameterDescription ( "io.out", "Output image containing class labels" );
    //SetParameterOutputImagePixelType( "out", ImagePixelType_uint8);


    // Elevation
// ElevationParametersHandler::AddElevationParameters(this, "elev");

    //Group Sample list
    AddParameter ( ParameterType_Group, "sample", "Training and validation samples parameters" );
    SetParameterDescription ( "sample",
                              "This group of parameters allows to set training and validation sample lists parameters." );

    AddParameter ( ParameterType_Int, "sample.mt", "Maximum training sample size per class" );
    //MandatoryOff("mt");
    SetDefaultParameterInt ( "sample.mt", 1000 );
    SetParameterDescription ( "sample.mt", "Maximum size per class (in pixels) of the training sample list (default = 1000) (no limit = -1). If equal to -1, then the maximal size of the available training sample list per class will be equal to the surface area of the smallest class multiplied by the training sample ratio." );
    AddParameter ( ParameterType_Int, "sample.mv", "Maximum validation sample size per class" );
    // MandatoryOff("mv");
    SetDefaultParameterInt ( "sample.mv", 1000 );
    SetParameterDescription ( "sample.mv", "Maximum size per class (in pixels) of the validation sample list (default = 1000) (no limit = -1). If equal to -1, then the maximal size of the available validation sample list per class will be equal to the surface area of the smallest class multiplied by the validation sample ratio." );

    AddParameter ( ParameterType_Empty, "sample.edg", "On edge pixel inclusion" );
    SetParameterDescription ( "sample.edg",
                              "Takes pixels on polygon edge into consideration when building training and validation samples." );
    MandatoryOff ( "sample.edg" );

    AddParameter ( ParameterType_Float, "sample.vtr", "Training and validation sample ratio" );
    SetParameterDescription ( "sample.vtr",
                              "Ratio between training and validation samples (0.0 = all training, 1.0 = all validation) (default = 0.5)." );
    SetParameterFloat ( "sample.vtr", 0.0 );



    AddParameter ( ParameterType_String, "sample.vfn", "Name of the discrimination field" );
    SetParameterDescription ( "sample.vfn", "Name of the field used to discriminate class labels in the input vector data files." );
    SetParameterString ( "sample.vfn", "Class" );



    AddParameter ( ParameterType_Int, "nbcomp", "Number of components (features) to keep by theme" );
    SetDefaultParameterInt ( "nbcomp", -1 );
    SetParameterDescription ( "nbcomp", "Number of components (features) to keep by theme (default = 0). If equal to 0, then all features are keps." );

    AddParameter ( ParameterType_Choice, "method", "Feature selection method to use" );
    SetParameterDescription ( "method", "Choice of the feature selection method to use." );

    AddRANDParameter();
    AddRAMParameter();



#ifdef OTB_USE_OPENCV
    InitRandomForestsParams();
#endif

    InitFSTParams();





    // Doc example parameter settings
    SetDocExampleParameterValue ( "io.il", "QB_1_ortho.tif" );
    SetDocExampleParameterValue ( "io.vd", "VectorData_QB1.shp" );
    SetDocExampleParameterValue ( "io.imstat", "EstimateImageStatisticsQB1.xml" );
    SetDocExampleParameterValue ( "sample.mv", "100" );
    SetDocExampleParameterValue ( "sample.mt", "100" );
// SetDocExampleParameterValue("sample.vtr", "0.5");
    SetDocExampleParameterValue ( "sample.edg", "false" );
    SetDocExampleParameterValue ( "sample.vfn", "Class" );
    SetDocExampleParameterValue ( "io.out", "QB_1_ortho.tif" );
}

void FeatureSelection::DoUpdateParameters()
{
    // Nothing to do here : all parameters are independent
}



//////////////

bool ImportanceVectorSort ( FeatureSelection::ImportancePairType a,FeatureSelection::ImportancePairType b )
{
    return a.second > b.second;
}

///////
FeatureSelection::ImportanceVector FeatureSelection::getFeatureImportance ( ListSampleType::Pointer trainingListSample,
        LabelListSampleType::Pointer trainingLabeledListSample )
{
    otbAppLogINFO ( "getFeatureImportance" );

    const std::string method = GetParameterString ( "method" );

    ImportanceVector vImp;

#ifdef OTB_USE_OPENCV
    if ( method == "rf" )
    {

        otbAppLogINFO ( "compute RF" );
        vImp = getFeatureImportanceRandomForests ( trainingListSample, trainingLabeledListSample );
    }
#endif

    if (method=="sffs") 
    {
        otbAppLogINFO("compute SFFS");
        vImp = getFeatureImportanceSFFS ( trainingListSample, trainingLabeledListSample );
    }

     else if (method=="dos") 
    {
        otbAppLogINFO("compute DOS");
        vImp = getFeatureImportanceDOS ( trainingListSample, trainingLabeledListSample );
    }
    
        
    
    sort ( vImp.begin(), vImp.end(), ImportanceVectorSort );
    
    return vImp;

}

///////


std::vector< std::string > FeatureSelection::getBandList ( std::vector< string > images )
{

    otb::ImageFileReader<FloatVectorImageType>::Pointer reader  = otb::ImageFileReader<FloatVectorImageType>::New();

    std::vector<std::string>  BandList;

    for ( std::vector<std::string>::iterator it = images.begin(); it != images.end() ; it ++ )
    {
        reader->SetFileName ( ( *it ).c_str() );
        reader->GenerateOutputInformation();

        int ncpp = reader->GetOutput()->GetNumberOfComponentsPerPixel();
        if ( ncpp == 1 )
        {
            BandList.push_back ( *it );
        }
        else
        {
            for ( int i=1; i <= ncpp; i++ )
            {
                std::ostringstream nbrband;
                nbrband << i;
                BandList.push_back ( ( *it ) + " band " + nbrband.str() );
            }
        }
    }

    return BandList;
}


void FeatureSelection::DoExecute()
{
    GetLogger()->Debug ( "Entering DoExecute\n" );


    unsigned int nbBands = 0;
    unsigned int nbImages = 0;


    //On recupère les images
    m_inList = GetParameterImageList ( "io.il" ); //input images list
    nbImages = m_inList->Size();


    VectorDataListType::Pointer vectorDataList = GetParameterVectorDataList ( "io.vd" );

    vdreproj = VectorDataReprojectionType::New();

    // read the Vectordata
    VectorDataType::Pointer vectorData = vectorDataList->GetNthElement ( 0 );
    vectorData->Update();

    // Setup the DEM Handler
    // otb::Wrapper::ElevationParametersHandler::SetupDEMHandlerFromElevationParameters(this, "elev");


    //Sample list generator
    ListSampleGeneratorType::Pointer sampleGenerator = ListSampleGeneratorType::New();

    if ( nbImages == 0 )
    {
        itkExceptionMacro ( "No input Image set..." );
    }

    //Get first image
    // FloatVectorImageType::Pointer image;
    FloatVectorImageType::Pointer image;


    if ( nbImages == 1 )
    {
        image = m_inList->GetNthElement ( 0 );
        image->UpdateOutputInformation();
        nbBands = image->GetNumberOfComponentsPerPixel();

        vdreproj->SetInputImage ( image );
        sampleGenerator->SetInput ( image );

    }


    // Split each input list  image into image
    // and generate an mono channel image list
    if ( nbImages > 1 )
    {

        m_inList->GetNthElement ( 0 )->UpdateOutputInformation();
        FloatVectorImageType::SizeType size = m_inList->GetNthElement ( 0 )->GetLargestPossibleRegion().GetSize();

        m_Concatener = ListConcatenerFilterType::New();
        m_ExtractorList = ExtractROIFilterListType::New();
        m_ImageList = ImageListType::New();

        for ( unsigned int i=0; i<m_inList->Size(); i++ )
        {
            FloatVectorImageType::Pointer vectIm = m_inList->GetNthElement ( i );
            vectIm->UpdateOutputInformation();


            if ( size != vectIm->GetLargestPossibleRegion().GetSize() )
            {
                GetLogger()->Debug ( "Input Image size mismatch...\n" );
                GetLogger()->Debug ( "This image is skipped : \n" );
                //itkExceptionMacro("Input Image size mismatch...");
            }
            else
            {
                for ( unsigned int j=0; j< vectIm->GetNumberOfComponentsPerPixel(); j++ )
                {
                    nbBands++;
                    MMExtractROIFilterType::Pointer extractor = MMExtractROIFilterType::New();
                    extractor->SetInput ( vectIm );
                    extractor->SetChannel ( j+1 );
                    //extractor->UpdateOutputInformation();
                    m_ExtractorList->PushBack ( extractor );
                    m_ImageList->PushBack ( extractor->GetOutput() );
                }
            }
        }

        otbAppLogINFO ( "Number of bands : " << nbBands );
        m_Concatener->SetInput ( m_ImageList );
        m_Concatener->UpdateOutputInformation();

        vdreproj->SetInputImage ( m_Concatener->GetOutput() );

        sampleGenerator->SetInput ( m_Concatener->GetOutput() );
    }



    vdreproj->SetInput ( vectorData );
    vdreproj->SetUseOutputSpacingAndOriginFromImage ( false );
    vdreproj->Update();

    sampleGenerator->SetInputVectorData ( vdreproj->GetOutput() );
    sampleGenerator->SetClassKey ( GetParameterString ( "sample.vfn" ) );
    sampleGenerator->SetMaxTrainingSize ( GetParameterInt ( "sample.mt" ) );
    sampleGenerator->SetMaxValidationSize ( GetParameterInt ( "sample.mv" ) );
    sampleGenerator->SetValidationTrainingProportion ( GetParameterFloat ( "sample.vtr" ) );


    // take pixel located on polygon edge into consideration
    if ( IsParameterEnabled ( "sample.edg" ) )
    {
        sampleGenerator->SetPolygonEdgeInclusion ( true );
    }

    sampleGenerator->Update();



    //To rescale data (or not)
    MeasurementType meanMeasurementVector;
    MeasurementType stddevMeasurementVector;
    if ( IsParameterEnabled ( "io.imstat" ) )
    {
        StatisticsReader::Pointer statisticsReader = StatisticsReader::New();
        statisticsReader->SetFileName ( GetParameterString ( "io.imstat" ) );
        meanMeasurementVector = statisticsReader->GetStatisticVectorByName ( "mean" );
        stddevMeasurementVector = statisticsReader->GetStatisticVectorByName ( "stddev" );
    }
    else
    {
        meanMeasurementVector.SetSize ( nbBands );
        meanMeasurementVector.Fill ( 0. );
        stddevMeasurementVector.SetSize ( nbBands );
        stddevMeasurementVector.Fill ( 1. );
    }


    ExtractROIFilterType::Pointer extractROIFilter = ExtractROIFilterType::New();

    if ( nbImages == 1 )
    {
        image->UpdateOutputInformation();
        extractROIFilter->SetStartX ( image->GetLargestPossibleRegion().GetIndex ( 0 ) );
        extractROIFilter->SetStartY ( image->GetLargestPossibleRegion().GetIndex ( 1 ) );
        extractROIFilter->SetSizeX ( image->GetLargestPossibleRegion().GetSize ( 0 ) );
        extractROIFilter->SetSizeY ( image->GetLargestPossibleRegion().GetSize ( 1 ) );
        extractROIFilter->SetInput ( image );
    }
    else
    {
        m_Concatener->GetOutput()->UpdateOutputInformation();
        extractROIFilter->SetStartX ( m_Concatener->GetOutput()->GetLargestPossibleRegion().GetIndex ( 0 ) );
        extractROIFilter->SetStartY ( m_Concatener->GetOutput()->GetLargestPossibleRegion().GetIndex ( 1 ) );
        extractROIFilter->SetSizeX ( m_Concatener->GetOutput()->GetLargestPossibleRegion().GetSize ( 0 ) );
        extractROIFilter->SetSizeY ( m_Concatener->GetOutput()->GetLargestPossibleRegion().GetSize ( 1 ) );
        extractROIFilter->SetInput ( m_Concatener->GetOutput() );
    }


    otbAppLogINFO ( " random training" );
    ListSampleType::Pointer trainingSample = ListSampleType::New();
    LabelListSampleType::Pointer trainingLabel = LabelListSampleType::New();


    trainingSample = sampleGenerator->GetTrainingListSample();
    trainingLabel = sampleGenerator->GetTrainingListLabel();


    // Shift scale the samples
    ShiftScaleFilterType::Pointer trainingShiftScaleFilter = ShiftScaleFilterType::New();
    trainingShiftScaleFilter->SetInput ( trainingSample );
    trainingShiftScaleFilter->SetShifts ( meanMeasurementVector );
    trainingShiftScaleFilter->SetScales ( stddevMeasurementVector );
    trainingShiftScaleFilter->Update();



    otbAppLogINFO ( "Number of training samples: " << trainingSample->Size() );
    otbAppLogINFO ( "Number of training labels : " << trainingLabel->Size() );



    //--------------------------
    // impprtance
    //--------------------------

    int nbcomp = -1;

    if ( IsParameterEnabled ( "nbcomp" ) )
        nbcomp = GetParameterInt ( "nbcomp" );

    nbcomp = ( nbcomp<0 ) ?nbBands:nbcomp; // si -1 on prend tous les canaux
    nbcomp = ( nbcomp>nbBands ) ?nbBands:nbcomp; // on se limite au nb de canaux de l'image !
    otbAppLogINFO ( "Numbers of bands to keep: " << nbcomp );

    otbAppLogINFO ( "Getting variabel importance " );

    ImportanceVector vImp = getFeatureImportance ( trainingSample, trainingLabel );


    // get input images list as  list -> to print feature selection result
    imagesBandList = getBandList ( GetParameterStringList ( "io.il" ) );

    for ( int i =0; i<nbcomp; i++ )
    {
        if ( vImp[i].second > 0 )
        {
            extractROIFilter->SetChannel ( vImp[i].first );

            otbAppLogINFO ( "keep channel number: " << vImp[i].first  <<" "<< imagesBandList[ vImp[i].first -1 ]  <<std::endl ); // vector start from 0
        }
    }

    otbAppLogINFO ( "Start writing image " );

    //On sauvegarde l'image
    
    extractROIFilter->Update();
        
    SetParameterOutputImage ( "io.out", extractROIFilter->GetOutput() );
    
    
    /**
        //On sauvegarde l'image
    typedef otb::ImageFileWriter<FloatVectorImageType> 	WriterType;
     WriterType::Pointer writer = WriterType::New();
      const std::string outputBaseName = GetParameterString ( "io.out" );
      
    writer->SetFileName(outputBaseName );
    otbAppLogINFO("Write file "+outputBaseName );
    writer->SetInput(extractROIFilter->GetOutput());
    writer->Update();
    **/

}


}
}

OTB_APPLICATION_EXPORT ( otb::Wrapper::FeatureSelection )
