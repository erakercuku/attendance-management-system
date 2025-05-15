#include "qr_code.hpp"
#include "qr/qrcodegen.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using qrcodegen::QrCode;
using qrcodegen::QrSegment;


void generate_and_show_qr(const std::string &id)
{
    const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW;
    QrCode qr = QrCode::encodeText(id.c_str(), errCorLvl);

    const int moduleSize = 10; // Scale: 10x10 pixels per module
    const int border = 4;      // Border modules

    const int size = qr.getSize();
    const int imageSize = (size + border * 2) * moduleSize;

    std::string filename = "temp_qr.pbm";
    std::ofstream file(filename);
    if (!file)
    {
        std::cerr << "Failed to create QR code image file.\n";
        return;
    }

    // PBM header
    file << "P1\n" << imageSize << " " << imageSize << "\n";

    for (int y = -border; y < size + border; ++y)
    {
        for (int v = 0; v < moduleSize; ++v)
        { // vertical scaling
            for (int x = -border; x < size + border; ++x)
            {
                bool black = (x >= 0 && x < size && y >= 0 && y < size && qr.getModule(x, y));
                for (int h = 0; h < moduleSize; ++h)
                { // horizontal scaling
                    file << (black ? "1 " : "0 ");
                }
            }
            file << "\n";
        }
    }

    file.close();

    std::cout << "QR code saved to: " << filename << "\n";
    std::cout << "Please open this file with an image viewer to view your QR code.\n";

    // Automatically open with default app (Windows only)
    std::system(("start " + filename).c_str());
}
