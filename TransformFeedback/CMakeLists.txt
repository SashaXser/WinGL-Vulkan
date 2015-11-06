cmake_minimum_required(VERSION 3.0.0)

set(TRANSFORM_FEEDBACK_SRC
Main.cpp
TransformFeedbackWindow.cpp
TransformFeedbackWindow.h
transform_feedback_gen.geom
transform_feedback_gen.vert
transform_feedback_vis.frag
transform_feedback_vis.vert
)

add_executable(TransformFeedback WIN32 ${TRANSFORM_FEEDBACK_SRC})

target_link_libraries(TransformFeedback WinGL)

copy_to_bin(transform_feedback_gen.geom)
copy_to_bin(transform_feedback_gen.vert)
copy_to_bin(transform_feedback_vis.frag)
copy_to_bin(transform_feedback_vis.vert)
