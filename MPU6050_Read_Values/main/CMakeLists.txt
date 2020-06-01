set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.cpp" 
"kalmanfilter.cpp" "MPU6050.cpp" "i2c.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
