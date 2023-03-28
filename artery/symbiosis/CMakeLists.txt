add_artery_feature(symbiosis
    CentralDataStorage.cc
    LteD2DInterface.cc
    FeudalisticPlatooningCentral.cc
)

generate_opp_message(Wrapper.msg TARGET symbiosis DIRECTORY symbiosis_msgs)
generate_opp_message(Wrapper.msg TARGET core DIRECTORY symbiosis_msgs)