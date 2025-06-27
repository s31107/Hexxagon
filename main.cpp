#include "MainWindow/MainWindow.h"


int main() {
    MainWindow window{};
    while (window.isOpen()) {
        window.eventManagement();
        window.show();
    }
}
