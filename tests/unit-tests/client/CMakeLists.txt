list(APPEND UNIT_TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_aging_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_stream_transport.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_buffer_vault.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client_platform.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client_mir_surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_mir_connection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_wait_handle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client_display_conf.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_mir_screencast.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_protobuf_rpc_channel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_probing_client_platform_factory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_mir_prompt_session.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_event_distributor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_probing_client_platform_factory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_periodic_perf_report.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client_buffer_stream.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_screencast_stream.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_connection_resource_map.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_presentation_chain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_mir_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_error_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client_mir_error.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_no_tls_future.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_mir_render_surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_frame_clock.cpp
)

if (NOT MIR_DISABLE_INPUT)
add_subdirectory("input")
endif()

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
