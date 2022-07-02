include(FetchContent)

FetchContent_Declare(quicr
        GIT_REPOSITORY      git@github.com:Quicr/libquicr.git
        GIT_TAG             main)
FetchContent_MakeAvailable(quicr)
