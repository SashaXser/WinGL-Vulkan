cmake_minimum_required(VERSION 3.0.0)

set(TRANSFORM_FEEDBACK_SRC
Main.cpp
TransformFeedbackWindow.cpp
TransformFeedbackWindow.h
transform_feedback.frag
transform_feedback.geom
transform_feedback.vert
)

add_executable(TransformFeedback WIN32 ${TRANSFORM_FEEDBACK_SRC})

target_link_libraries(TransformFeedback WinGL)

copy_to_bin(transform_feedback.vert)
copy_to_bin(transform_feedback.geom)
copy_to_bin(transform_feedback.frag)
