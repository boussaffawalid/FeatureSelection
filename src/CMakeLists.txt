set( FeatureSelection_SRC otbFeatureSelection.cxx otbSelectFST.cxx )


if(OTB_USE_OPENCV)
  # Add source files with OpenCV dependency
  set( FeatureSelection_SRC ${FeatureSelection_SRC}
  			 otbSelectRandomForests.cxx )
endif()


OTB_CREATE_APPLICATION(NAME         FeatureSelection
                       SOURCES      ${FeatureSelection_SRC}
                       LINK_LIBRARIES ${OTB_LIBRARIES})
                        
 
