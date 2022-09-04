# aux_source_directory(. SRC_LIST)
set(SRC_LIST 
    myrpcApplication.cc 
    myrpcConfig.cc 
    myrpcHeader.pb.cc 
    myrpcProvider.cc
    myrpcChannel.cc
    myrpcController.cc
    myrpcLog.cc)

add_library(myrpc ${SRC_LIST})

target_link_libraries(myrpc muduo_net muduo_base pthread)