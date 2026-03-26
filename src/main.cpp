#include "MinecraftClient.h"

int main() {
    MinecraftClient minecraft(App::CreatePlatform());

    minecraft.run();

    return 0;
}