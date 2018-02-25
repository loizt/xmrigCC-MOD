#include "App.h"

int main(int argc, char **argv) {
    App* app = new App(argc, argv);
    int res = app->start();
    delete app;

    return res;
}
