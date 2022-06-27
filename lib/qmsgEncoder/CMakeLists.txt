# Define the QmsgEnoder project, inheriting the version from the root
# of the QMsg project
project(QMsgEncoder
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Qmsg Encoder Library"
        LANGUAGES CXX)

add_subdirectory(src)
