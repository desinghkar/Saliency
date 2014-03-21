cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(dir_files src/dir_files.cpp)
rosbuild_add_executable(depthSaliencyFiles src/depthSaliencyFiles.cpp src/saliency_routines.cpp src/dir_files.cpp)
rosbuild_add_executable(region_ground src/region_ground.cpp src/dir_files.cpp)
rosbuild_add_executable(get_fixation_vs_depth src/get_fixation_vs_depth.cpp src/saliency_routines.cpp src/dir_files.cpp)
#rosbuild_add_executable(blurring_exp src/blurring_exp.cpp src/saliency_routines.cpp src/dir_files.cpp)
rosbuild_add_executable(imageSaliencyFiles src/imageSaliencyFiles.cpp src/dir_files.cpp)
rosbuild_add_executable(gtSaliencyFiles src/gtSaliencyFiles.cpp src/dir_files.cpp)
rosbuild_add_executable(svmCompatibleFileGeneration src/svmCompatibleFileGeneration.cpp src/dir_files.cpp)
rosbuild_add_executable(svmTrainTestFilesGeneration src/svmTrainTestFilesGeneration.cpp src/dir_files.cpp)
rosbuild_add_executable(svmTrainImages src/svmTrainImages.cpp)
rosbuild_add_executable(forROC src/forROC.cpp)
rosbuild_add_executable(beforeROC src/beforeROC.cpp)
rosbuild_add_executable(randomsampling_rgbd_features src/randomsampling_rgbd_features.cpp)
rosbuild_add_executable(mergingSaliencyValuesForTraining src/mergingSaliencyValuesForTraining.cpp)
#target_link_libraries(example ${PROJECT_NAME})
