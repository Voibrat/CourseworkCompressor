#include <zstd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

bool compress_file(const std::string& srcFile, const std::string& dstFile)
{
    std::ifstream inputFile(srcFile, std::ios::binary);
    if (!inputFile)
    {
        std::cerr << "Failed to open source file!" << std::endl;
        return false;
    }

    std::ofstream outputFile(dstFile, std::ios::binary);
    if (!outputFile)
    {
        std::cerr << "Failed to create compressed file!" << std::endl;
        return false;
    }

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx)
    {
        std::cerr << "Failed to create compression context!" << std::endl;
        return false;
    }

    const size_t bufferSize = ZSTD_CStreamOutSize();
    std::vector<char> inBuffer(bufferSize);
    std::vector<char> outBuffer(bufferSize);

    ZSTD_inBuffer input = { inBuffer.data(), 0, 0 };
    ZSTD_outBuffer output = { outBuffer.data(), outBuffer.size(), 0 };

    ZSTD_compressBegin(cctx);
    while (inputFile)
    {
        inputFile.read(input.buffer, bufferSize);
        input.size = inputFile.gcount();
        input.pos = 0;

        while (input.pos < input.size)
        {
            ZSTD_compressStream(cctx, &output, &input);
            if (output.pos > 0)
            {
                outputFile.write(output.buffer, output.pos);
                output.pos = 0;
            }
        }
    }

    ZSTD_compressEnd(cctx);
    ZSTD_freeCCtx(cctx);

    return true;
}

bool decompress_file(const std::string& srcFile, const std::string& dstFile)
{
    std::ifstream inputFile(srcFile, std::ios::binary);
    if (!inputFile)
    {
        std::cerr << "Failed to open compressed file!" << std::endl;
        return false;
    }

    std::ofstream outputFile(dstFile, std::ios::binary);
    if (!outputFile)
    {
        std::cerr << "Failed to create decompressed file!" << std::endl;
        return false;
    }

    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    if (!dctx)
    {
        std::cerr << "Failed to create decompression context!" << std::endl;
        return false;
    }

    const size_t bufferSize = ZSTD_DStreamOutSize();
    std::vector<char> inBuffer(bufferSize);
    std::vector<char> outBuffer(bufferSize);

    ZSTD_inBuffer input = { inBuffer.data(), 0, 0 };
    ZSTD_outBuffer output = { outBuffer.data(), outBuffer.size(), 0 };

    ZSTD_decompressBegin(dctx);
    while (inputFile)
    {
        inputFile.read(input.buffer, bufferSize);
        input.size = inputFile.gcount();
        input.pos = 0;

        while (input.pos < input.size)
        {
            ZSTD_decompressStream(dctx, &output, &input);
            if (output.pos > 0)
            {
                outputFile.write(output.buffer, output.pos);
                output.pos = 0;
            }
        }
    }

    ZSTD_decompressEnd(dctx);
    ZSTD_freeDCtx(dctx);

    return true;
}

int main()
{
    const std::string srcFile = "test.txt";
    const std::string compressedFile = "compressed.bin";
    const std::string decompressedFile = "decompressed.txt";

    if (!compress_file(srcFile, compressedFile))
    {
        std::cerr << "Compression failed!" << std::endl;
        return 1;
    }

    if (!decompress_file(compressedFile, decompressedFile))
    {
        std::cerr << "Decompression failed!" << std::endl;
        return 1;
    }

    std::cout << "Compression and decompression completed successfully!" << std::endl;

    return 0;
}