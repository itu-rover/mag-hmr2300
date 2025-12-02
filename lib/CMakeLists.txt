add_library(hmr2300)

target_include_directories(hmr2300 PUBLIC include)

target_sources(hmr2300 PRIVATE
    src/hmr2300.c
)

install(TARGETS hmr2300)
