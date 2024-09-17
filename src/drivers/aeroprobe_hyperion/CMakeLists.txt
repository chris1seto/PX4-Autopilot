px4_add_module(
	MODULE drivers__aeroprobe_hyperion
	MAIN aeroprobe_hyperion
	COMPILE_FLAGS
	SRCS
		AeroprobeHyperion.cpp
    QueueBuffer.cpp
    AeroprobeHyperionParser.cpp
    HrtHelper.cpp
	)