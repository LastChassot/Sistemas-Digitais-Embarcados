# Este é o CMakeLists.txt de DENTRO da pasta 'main'

idf_component_register(
        # Adicione TODOS os seus arquivos (.cpp e .h) aqui.
        # O build do IDF é inteligente e saberá o que fazer.
        # Isso ajuda o CLion a indexar tudo corretamente.
        SRCS
        "src/main.cpp"
        "src/DisplayManager.cpp"
        "src/ContadorController.cpp"
        "include/ContadorController.h"
        "include/DisplayConfig.h"
        "include/DisplayManager.h"

        # O diretório de include ainda é necessário para o compilador
        # encontrar os #includes (caminho relativo a este arquivo)
        INCLUDE_DIRS
        "include"

        # Dependências (precisamos do 'driver' para GPIO e UART)
        PRIV_REQUIRES
        driver
)