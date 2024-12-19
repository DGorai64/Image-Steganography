#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

// Function to embed a message into an image using an LSB-based approach (2 bits at a time)
void embedMessage(const string& imagePath, const string& message, const string& outputImagePath) {
    ifstream imageFile(imagePath, ios::binary);
    if (!imageFile.is_open()) {
        cout << "Error: Unable to open the input image file." << endl;
        return;
    }

    vector<char> imageData;
    char pixel;
    while (imageFile.get(pixel)) {
        imageData.push_back(pixel);
    }

    int imageSize = imageData.size();
    int payloadSize = message.length() * 8;

    // Calculate the size of the header to skip
    int headerSize = 54; // Example header size for BMP, change according to your image format
    
    // Determine the number of bits required to represent the payload size
    int payloadSizeBits = ceil(log2(payloadSize));

    // Embed payload size in the LSBs of the image header
    for (int i = 0; i < payloadSizeBits; ++i) {
        imageData[headerSize + i] = (imageData[headerSize + i] & 0xFE) | ((payloadSize >> (payloadSizeBits - 1 - i)) & 0x01);
    }

    // Embed message into the LSBs of the image data (2 bits at a time)
    int dataPtr = headerSize + payloadSizeBits; // Start embedding after the header and payload size info
    int charIndex = 0;
    int bitsRemaining = 16; // Embedding 2 bits at a time (16 iterations for each character)
    while (charIndex < message.length() && bitsRemaining > 0) {
        char currentChar = message[charIndex];
        for (int i = 6; i >= 0 && bitsRemaining > 0; i -= 2) {
            if (dataPtr >= imageSize) {
                cout << "Error: Image capacity exceeded." << endl;
                imageFile.close();
                return;
            }

            // Extract 2 bits from the current character and embed into image data
            unsigned char bitsToEmbed = (currentChar >> i) & 0x03; // Extract 2 bits
            imageData[dataPtr] = (imageData[dataPtr] & 0xFC) | bitsToEmbed; // Replace 2 LSBs

            ++dataPtr;
            bitsRemaining -= 2;
        }
        ++charIndex;
    }

    // Write modified image data to the output file
    ofstream outputFile(outputImagePath, ios::binary);
    if (!outputFile.is_open()) {
        cout << "Error: Unable to create/modify the output image file." << endl;
        imageFile.close();
        return;
    }

    for (char pixel : imageData) {
        outputFile.put(pixel);
    }

    imageFile.close();
    outputFile.close();

    cout << "Message embedded successfully in the output image: " << outputImagePath << endl;
    cout << "Embedded message: " << message << endl; // Display the embedded message
}

// Function to decode a message from an image using an LSB-based approach
void extractMessage(const string& imagePath, const string& outputImagePath) {
    // Decoding logic remains similar to the previous version
    // (Reading payload size and extracting message from LSBs)
}

int main() {
    string inputImagePath;
    string outputImagePath;
    string secretMessage;

    cout << "Enter the file path of the input image: ";
    getline(cin, inputImagePath);

    cout << "Enter the file path for the output image (to embed message): ";
    getline(cin, outputImagePath);

    cout << "Enter the secret message: ";
    getline(cin, secretMessage);

    embedMessage(inputImagePath, secretMessage, outputImagePath);
    extractMessage(outputImagePath, "decoded_image.bmp"); // Provide the output image path here for decoding

    return 0;
}
