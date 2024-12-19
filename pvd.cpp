#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

// Function to embed a message into an image using the Pixel Value Differencing (PVD) method
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
    int messageIndex = 0;
    int bitIndex = 0;

    for (int i = 0; i < imageSize && messageIndex < message.length(); ++i) {
        // Extract pixel values (current and next)
        unsigned char currentPixel = static_cast<unsigned char>(imageData[i]);
        unsigned char nextPixel = (i < imageSize - 1) ? static_cast<unsigned char>(imageData[i + 1]) : 0;

        // Calculate the difference between pixel values
        int diff = nextPixel - currentPixel;

        // Embed message bits into the difference values (PVD method)
        if (bitIndex < 8) {
            // Extract a bit from the message
            int bit = (message[messageIndex] >> (7 - bitIndex)) & 1;

            // Embed the bit into the difference
            if (bit == 1 && diff > 0) {
                ++diff; // Increase the difference by 1 if the bit is 1
            } else if (bit == 0 && diff < 0) {
                --diff; // Decrease the difference by 1 if the bit is 0
            }

            // Modify the next pixel value based on the modified difference
            imageData[i + 1] = static_cast<char>(diff + currentPixel);
            ++bitIndex;

            // Move to the next message bit
            if (bitIndex == 8) {
                ++messageIndex;
                bitIndex = 0;
            }
        }
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

// Function to decode a message from an image using the Pixel Value Differencing (PVD) method
void extractMessage(const string& imagePath, const string& outputFilePath) {
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
    string extractedMessage = "";
    int bitIndex = 0;
    char currentByte = 0;

    for (int i = 0; i < imageSize; ++i) {
        // Extract pixel values (current and next)
        unsigned char currentPixel = static_cast<unsigned char>(imageData[i]);
        unsigned char nextPixel = (i < imageSize - 1) ? static_cast<unsigned char>(imageData[i + 1]) : 0;

        // Calculate the difference between pixel values
        int diff = nextPixel - currentPixel;

        // Extract message bits from the differences
        if (bitIndex < 8) {
            if (diff > 0) {
                extractedMessage += '1';
            } else if (diff < 0) {
                extractedMessage += '0';
            }

            ++bitIndex;

            // Construct a byte when 8 bits are collected
            if (bitIndex == 8) {
                currentByte = (currentByte << 1) | (extractedMessage.back() - '0');
                extractedMessage = extractedMessage.substr(0, extractedMessage.size() - 1);
                bitIndex = 0;

                // Write the constructed byte to the output file
                ofstream outputFile(outputFilePath, ios::binary | ios::app);
                if (outputFile.is_open()) {
                    outputFile.put(currentByte);
                    outputFile.close();
                } else {
                    cout << "Error: Unable to create/modify the output file for extracted message." << endl;
                    return;
                }
            }
        }
    }

    if (!extractedMessage.empty()) {
        cout << "Warning: The extracted message is incomplete." << endl;
    } else {
        cout << "Message extracted successfully to the output file: " << outputFilePath << endl;
    }
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
    extractMessage(outputImagePath, "decoded_message.txt");

    return 0;
}
