#include "app.h"
#include "cli.h"

void printUsage();

int main(int argc, const char* argv[]) {
    ShadeApp app;
    
    bool verbose = false;
    const char* shaderFile;

    cli::Parser parser = {
        cli::OptionFlag('v', "verbose", "output logging info", &verbose)
    };

    if(!parser.parse(argc, argv)) {
        printUsage();
        parser.printOptionsUsage();
        return EXIT_FAILURE;
    }

    if(parser.getRemainingArgs().size() < 1) {
        // Use default shader
        shaderFile = NULL;
    } else {
        shaderFile = parser.getRemainingArgs()[0];
    }

    if(verbose) {
        loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
    } else {
        loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    }

    if(!app.init("Shade", 800, 600)) {
        return EXIT_FAILURE;
    }

    if(shaderFile)
        app.loadFragmentShader(shaderFile);

    return app.runLoop();
}


void printUsage() {
    fprintf(stderr, 
        "Usage: shade [options] [SHADER_FILE]\n\n"
        "    Renders the shader in a window.\n\n");
}