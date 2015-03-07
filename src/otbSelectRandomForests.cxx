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
void FeatureSelection::InitRandomForestsParams()
{
    AddChoice ( "method.rf", "Random forests feature selection" );
    SetParameterDescription ( "method.rf",
                              "This group of parameters allows to set Random Forests feature selection parameters. "
                              "See complete documentation here \\url{http://docs.opencv.org/modules/ml/doc/random_trees.html}." );
    //MaxDepth
    AddParameter ( ParameterType_Int, "method.rf.max", "Maximum depth of the tree" );
    SetParameterInt ( "method.rf.max", 5 );
    SetParameterDescription (
        "method.rf.max",
        "The depth of the tree. A low value will likely underfit and conversely a high value will likely overfit. "
        "The optimal value can be obtained using cross validation or other suitable methods." );

    //MinSampleCount
    AddParameter ( ParameterType_Int, "method.rf.min", "Minimum number of samples in each node" );
    SetParameterInt ( "method.rf.min", 10 );
    SetParameterDescription (
        "method.rf.min", "If the number of samples in a node is smaller than this parameter, "
        "then the node will not be split. A reasonable value is a small percentage of the total data e.g. 1 percent." );

    //RegressionAccuracy
    AddParameter ( ParameterType_Float, "method.rf.ra", "Termination Criteria for regression tree" );
    SetParameterFloat ( "method.rf.ra", 0. );
    SetParameterDescription ( "method.rf.ra", "If all absolute differences between an estimated value in a node "
                              "and the values of the train samples in this node are smaller than this regression accuracy parameter, "
                              "then the node will not be split." );

    //UseSurrogates : don't need to be exposed !
    //AddParameter(ParameterType_Empty, "method.rf.sur", "Surrogate splits will be built");
    //SetParameterDescription("method.rf.sur","These splits allow to work with missing data and compute variable importance correctly.");

    //MaxNumberOfCategories
    AddParameter ( ParameterType_Int, "method.rf.cat",
                   "Cluster possible values of a categorical variable into K <= cat clusters to find a suboptimal split" );
    SetParameterInt ( "method.rf.cat", 10 );
    SetParameterDescription (
        "method.rf.cat",
        "Cluster possible values of a categorical variable into K <= cat clusters to find a suboptimal split." );

    //Priors are not exposed.

    //CalculateVariableImportance not exposed

    //MaxNumberOfVariables
    AddParameter ( ParameterType_Int, "method.rf.var",
                   "Size of the randomly selected subset of features at each tree node" );
    SetParameterInt ( "method.rf.var", 0 );
    SetParameterDescription (
        "method.rf.var",
        "The size of the subset of features, randomly selected at each tree node, that are used to find the best split(s). "
        "If you set it to 0, then the size will be set to the square root of the total number of features." );

    //MaxNumberOfTrees
    AddParameter ( ParameterType_Int, "method.rf.nbtrees",
                   "Maximum number of trees in the forest" );
    SetParameterInt ( "method.rf.nbtrees", 100 );
    SetParameterDescription (
        "method.rf.nbtrees",
        "The maximum number of trees in the forest. Typically, the more trees you have, the better the accuracy. "
        "However, the improvement in accuracy generally diminishes and reaches an asymptote for a certain number of trees. "
        "Also to keep in mind, increasing the number of trees increases the prediction time linearly." );

    //ForestAccuracy
    AddParameter ( ParameterType_Float, "method.rf.acc",
                   "Sufficient accuracy (OOB error)" );
    SetParameterFloat ( "method.rf.acc", 0.01 );
    SetParameterDescription ( "method.rf.acc","Sufficient accuracy (OOB error)." );


    //TerminationCriteria not exposed
}

FeatureSelection::ImportanceVector FeatureSelection::getFeatureImportanceRandomForests ( ListSampleType::Pointer trainingListSample,
        LabelListSampleType::Pointer trainingLabeledListSample )
{

    RandomForestType::Pointer classifier = RandomForestType::New();
    classifier->SetInputListSample ( trainingListSample );
    classifier->SetTargetListSample ( trainingLabeledListSample );
    classifier->SetMaxDepth ( GetParameterInt ( "method.rf.max" ) );
    classifier->SetMinSampleCount ( GetParameterInt ( "method.rf.min" ) );
    classifier->SetRegressionAccuracy ( GetParameterFloat ( "method.rf.ra" ) );
    classifier->SetMaxNumberOfCategories ( GetParameterInt ( "method.rf.cat" ) );
    classifier->SetMaxNumberOfVariables ( GetParameterInt ( "method.rf.var" ) );
    classifier->SetMaxNumberOfTrees ( GetParameterInt ( "method.rf.nbtrees" ) );
    classifier->SetForestAccuracy ( GetParameterFloat ( "method.rf.acc" ) );

    classifier->SetCalculateVariableImportance ( true );

    classifier->Train();



#if OTB_VERSION_MAJOR == 4 &&  OTB_VERSION_MINOR < 4

    cv::Mat importance=classifier->GetVariableImportance();

    int dim_training_data = importance.cols;

    ImportanceVector vImp;
    vImp.reserve ( dim_training_data );
    for ( int n=0; n<dim_training_data; n++ )
        vImp.push_back ( std::make_pair ( n,importance.at<float> ( 0,n ) ) );


#else

    //On recupere l'importance des variables
    itk::VariableSizeMatrix<float>  importance = classifier->GetVariableImportance();

    int dim_training_data = importance.Cols();

    ImportanceVector vImp;
    vImp.reserve ( dim_training_data );

    for ( int n=1; n<=dim_training_data; n++ )
        vImp.push_back ( std::make_pair ( n, importance[0][n] ) );

#endif



    return vImp;
}

} //end namespace wrapper
} //end namespace otb
