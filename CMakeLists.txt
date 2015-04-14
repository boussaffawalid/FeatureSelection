PROJECT(AppFeatureSelection)

set(AppFeatureSelection_LIBRARIES OTBAppFeatureSelection)


set(FST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/FST3lib/_src_criteria
	${CMAKE_CURRENT_SOURCE_DIR}/extern/FST3lib/_src_dataio
	${CMAKE_CURRENT_SOURCE_DIR}/extern/FST3lib/_src_global
	${CMAKE_CURRENT_SOURCE_DIR}/extern/FST3lib/_src_search)
include_directories(${FST_INCLUDE_DIR})

otb_module_impl()

