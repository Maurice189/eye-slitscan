add_subdirectory(similarity-measures)
add_subdirectory(importer)
add_subdirectory(slitscan-generator)
add_subdirectory(clustering)

target_include_directories(sequence_gen PUBLIC .)
target_include_directories(mscanpath PUBLIC .)
target_include_directories(mtrajectory PUBLIC .)
target_include_directories(gen_slitscan PUBLIC .)
