cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(OBJECT_TRACKING)

FIND_PACKAGE( OpenCV REQUIRED )

# add platform-specific defines here so code can know which
# OS we're using (WIN32, UNIX, APPLE)
# :TODO:
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

add_library(libObjectTracking src/cv_onlineboosting.cpp include/cv_onlineboosting.h src/cv_onlinemil.cpp include/cv_onlinemil.h src/object_tracker.cpp include/object_tracker.h)
target_link_libraries(libObjectTracking ${OpenCV_LIBRARIES})

include_directories(include)

add_executable(ObjectTrackingApp samples/object_tracker_app.cpp)
include_directories(include)
add_dependencies(ObjectTrackingApp libObjectTracking)
target_link_libraries(ObjectTrackingApp ${OpenCV_LIBRARIES} libObjectTracking)
