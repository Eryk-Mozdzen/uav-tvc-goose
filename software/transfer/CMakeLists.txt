
add_library(transfer
	src/transfer.cpp
)

target_include_directories(transfer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(transfer PRIVATE m)
