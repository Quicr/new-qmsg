# Define the QMsgEnoder project, inheriting the version from the root
# of the QMsg project
project(QMsgEncoder
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "QMsg Encoder / Decoder Library"
        LANGUAGES CXX)

add_subdirectory(src)
