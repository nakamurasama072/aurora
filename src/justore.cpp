//
// Created by nakamurasama072 on 2026/7/25.
//
#include <restapi.hpp>

int main() {
    crow::SimpleApp justore_app;
    CrowRestAPI justore(justore_app, 12384);

    // Run the app
    justore.run();
    return 0;
}