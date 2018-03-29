cmake_minimum_required(VERSION 3.1)
project(gizmo)

set(libgizmo_sources_base_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/libgizmo")

if(WIN32)
	set(libgizmo_sources_glloadgen_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/glloadgen")
endif()

set(LIBGIZMO_SOURCES_CORE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/IGizmo.h
    ${libgizmo_sources_base_dir}/GizmoTransform.h
    ${libgizmo_sources_base_dir}/GizmoTransformMove.cpp
    ${libgizmo_sources_base_dir}/GizmoTransformMove.h
    ${libgizmo_sources_base_dir}/GizmoTransformRender.cpp
    ${libgizmo_sources_base_dir}/GizmoTransformRender.h
    ${libgizmo_sources_base_dir}/GizmoTransformRotate.cpp
    ${libgizmo_sources_base_dir}/GizmoTransformRotate.h
    ${libgizmo_sources_base_dir}/GizmoTransformScale.cpp
    ${libgizmo_sources_base_dir}/GizmoTransformScale.h
    ${libgizmo_sources_base_dir}/LibBase.h
    ${libgizmo_sources_base_dir}/ZBaseDefs.h
    ${libgizmo_sources_base_dir}/ZBaseMaths.cpp
    ${libgizmo_sources_base_dir}/ZBaseMaths.h
    ${libgizmo_sources_base_dir}/ZBaseMaths.inl
    ${libgizmo_sources_base_dir}/ZCollisionsUtils.h
    ${libgizmo_sources_base_dir}/ZMathsFunc.cpp
    ${libgizmo_sources_base_dir}/ZMathsFunc.h
)

add_library(${PROJECT_NAME} STATIC ${LIBGIZMO_SOURCES_CORE})

if(WIN32)
	include_directories(${libgizmo_sources_glloadgen_dir})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

if(WIN32)
	find_package(OpenGL REQUIRED)
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc" ${OPENGL_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})
endif()

if(EXAMPLE_GLFW)
  set(LIBGIZMO_SOURCES_EXAMPLE_GLFW ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw/main.cpp)
  add_executable(gizmo_glfw ${LIBGIZMO_SOURCES_EXAMPLE_GLFW})
  find_library(GLFW_LIBRARY glfw)
  target_link_libraries(gizmo_glfw ${GLFW_LIBRARY} ${PROJECT_NAME})
endif()

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

INSTALL(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/inc/IGizmo.h
	DESTINATION include/LibGizmo)
