((nil . ((projectile-project-compilation-cmd . "cmake -H. -Bbuild -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug && cmake --build build")
         (projectile-project-test-cmd . "cmake --build build -t tests && build/tests")
         (projectile-project-run-cmd . "cmake --build build && build/main")))
 (c++-mode . ((lsp-clients-clangd-args . ("--compile-commands-dir=build"
                                          "--pch-storage=memory"
                                          "--background-index"
                                          "-j=4"
                                          "--header-insertion=never"
                                          )))))

