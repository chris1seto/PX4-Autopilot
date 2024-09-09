px4_add_module(
	MODULE drivers__radical_can
	MAIN radical_can
	COMPILE_FLAGS
	SRCS
		RadicalCan.cpp
    SensataBms.cpp
	)
