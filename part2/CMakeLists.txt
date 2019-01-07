set(CMAKE_CXX_STANDARD 14)

add_executable(UnitTest UnitTest.cpp IProduct.cpp IShop.cpp)
add_executable(MultithreadingTest MultithreadingTest.cpp IProduct.cpp IShop.cpp)

target_link_libraries(UnitTest gtest gtest_main)
