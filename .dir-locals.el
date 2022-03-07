((nil . ((compile-command . "cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .")
         (projectile-project-compilation-cmd . "cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .")
         (projectile-project-test-cmd . "cd build && cmake .. && cmake --build . -t tests && ./bin/tests")
         (projectile-project-run-cmd . "cd build && cmake .. && cmake --build . && ./bin/main")))
 (c++-mode . ((lsp-clients-clangd-args . ("--compile-commands-dir=build"
                                          "--pch-storage=memory"
                                          "--background-index"
                                          "-j=4"
                                          "--header-insertion=never"
                                          )))))

