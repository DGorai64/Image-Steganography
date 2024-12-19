#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Function to hide a message in a BMP image using LSB steganography
void encodeLSB(const std::string& inputImage, const std::string& outputImage, const std::string& message) {
    // Open the input BMP image file
    std::ifstream inImage(inputImage, std::ios::binary);
    if (!inImage) {
        std::cerr << "Error opening input image. Please check the file path and permissions." << std::endl;
        return;
    }

    // Open the output BMP image file
    std::ofstream outImage(outputImage, std::ios::binary);
    if (!outImage) {
        std::cerr << "Error opening output image. Please check the file path and permissions." << std::endl;
        inImage.close();
        return;
    }

    // Read and write the BMP header
    char header[54];
    inImage.read(header, 54);
    outImage.write(header, 54);

    // Convert the message length to binary (32 bits)
    std::string lengthBinary;
    for (int i = 31; i >= 0; --i) {
        lengthBinary += ((message.length() >> i) & 1) ? '1' : '0';
    }

    // Encode the message length (32 bits) into the image
    int lengthIndex = 0;
    char pixel;
    size_t pixelIndex = 0;

    while (lengthIndex < 32) {
        inImage.get(pixel);

        char modifiedPixel = (pixel & 0xFE) | (lengthBinary[lengthIndex] - '0');
        outImage.put(modifiedPixel);

        lengthIndex++;
        pixelIndex++;
    }

    // Encode the message
    for (size_t i = 0; i < message.length(); i++) {
        for (int j = 7; j >= 0; j--) {
            inImage.get(pixel);

            char modifiedPixel = (pixel & 0xFE) | ((message[i] >> j) & 1);
            outImage.put(modifiedPixel);

            pixelIndex++;
        }
    }

    // Copy the remaining pixels from the input image to the output image
    while (inImage.get(pixel)) {
        outImage.put(pixel);
        pixelIndex++;
    }

    inImage.close();
    outImage.close();

    std::cout << "Message hidden successfully." << std::endl;
}

// Function to decode a hidden message from a BMP image using LSB steganography
std::string decodeLSB(const std::string& imageFile) {
    // Open the BMP image file for reading
    std::ifstream inImage(imageFile, std::ios::binary);
    if (!inImage) {
        std::cerr << "Error opening image file. Please check the file path and permissions." << std::endl;
        return "";
    }

    // Skip the BMP header (first 54 bytes)
    inImage.seekg(54, std::ios::beg);

    // Decode the message length (32 bits)
    std::string lengthBinary;
    for (int i = 0; i < 32; i++) {
        char pixel;
        inImage.get(pixel);
        lengthBinary += (pixel & 1) ? '1' : '0';
    }

    size_t messageLength = 0;
    for (int i = 0; i < 32; i++) {
        messageLength = (messageLength << 1) | (lengthBinary[i] - '0');
    }

    // Decode the message
    std::string message;
    char currentChar = 0;
    int bitIndex = 0;

    for (size_t i = 0; i < messageLength; i++) {
        for (int j = 0; j < 8; j++) {
            char pixel;
            inImage.get(pixel);
            currentChar = (currentChar << 1) | (pixel & 1);
            bitIndex++;

            if (bitIndex == 8) {
                message += currentChar;
                currentChar = 0;
                bitIndex = 0;
            }
        }
    }

    inImage.close();
    return message;
}

int main() {
    std::string inputImage;
    std::string outputImage;
    std::string message;

    // Specify the paths and message
    std::cout << "Enter the path to the input BMP image: ";
    std::cin >> inputImage;

    std::cout << "Enter the path to the output BMP image: ";
    std::cin >> outputImage;

    std::cin.ignore();

    std::cout << "Enter the message to hide: ";
    std::getline(std::cin, message);

    // Encode the message into the image
    encodeLSB(inputImage, outputImage, message);

    // Decode the hidden message from the stego image
    std::string decodedMessage = decodeLSB(outputImage);

    if (decodedMessage.empty()) {
        std::cout << "No hidden message found in the stego image." << std::endl;
    } else {
        std::cout << "Hidden message: " << decodedMessage << std::endl;
    }

    return 0;
}
